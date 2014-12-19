// 
// internal utility functions for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scribe_conf.h"
#include "scribe_debug.h"
#include "scribe_format.h"
#include "scribe_loglevel.h"
#include "scribe_metainfo.h"
#include "scribe_return_types.h"
#include "scribe_utils.h"

#define BUILDER_INIT_SIZE 128

static inline
char * reallocstr(char * str, bool const build_string_allocated, uint64_t const newcap)
{
#if defined(__cplusplus)
    return (build_string_allocated ? (char *) realloc(str, newcap * sizeof(char)) 
                                   : (char *) malloc(newcap * sizeof(char)));
#else
    return (build_string_allocated ? realloc(str, newcap * sizeof(char)) 
                                   : malloc(newcap * sizeof(char)));
#endif
}

static inline
int checklen(char ** const build_string, 
             char * const printbuff, 
             bool * const build_string_allocated,
             uint64_t * const build_string_cap, 
             char ** const writing_pos, 
             uint64_t * const written_length, 
             uint64_t const addlen)
{
    if (unlikely(*written_length + addlen >= *build_string_cap)) {
        *build_string = reallocstr(*build_string, *build_string_allocated, *build_string_cap * 2);
        if (NULL == *build_string) {
            goto error;
        }
        if (!(*build_string_allocated)) {
            memcpy(*build_string, printbuff, *written_length);
        }
        *build_string_allocated = true;
        *build_string_cap       = *build_string_cap * 2;
        *writing_pos            = *build_string + *written_length;
    }
    return (SCRB_Success);
error:
    return (SCRB_Failure);
}

char * scrb_build_msg(enum scrb_loglevel const severity,
                      char const * const streamname,
                      struct scrb_meta_info const mi, 
                      struct scrb_format const * const fmt, 
                      char * const printbuff, 
                      uint64_t const cap, 
                      char const * const msg, 
                      uint64_t * const total_length,
                      bool const newline)
{
    char * build_string         = printbuff;
    char * writing_pos          = build_string;
    char const * fmtstr         = fmt->fmtstr;
    bool build_string_allocated = false;
    bool const hastimehook      = (NULL != fmt->timehook);
    uint64_t build_string_cap   = cap;
    uint64_t const numfmts      = fmt->numfmts;

    // we'll copy the format types into a static array to remove one level
    // of pointer indirection to pass through during the loop below.
    fmttype const fmttypes[numfmts];
    memcpy((void *)fmttypes, fmt->fmttypes, numfmts * sizeof(fmttype));

    uint64_t written_length = 0, fmtcount = 0;
    char writebuffer[64];
    while (*fmtstr != '\0') { 
        // oops, looks like we've hit the write capacity,
        // we'll allocate a new buffer and work from there.
        if (unlikely(written_length == build_string_cap)) {
            build_string = reallocstr(build_string, build_string_allocated, 2 * build_string_cap);
            if (NULL == build_string) {
                goto error;
            }
            if (!build_string_allocated) {
                memcpy(build_string, printbuff, written_length);
            }
            build_string_allocated = true;
            build_string_cap      *= 2;
            writing_pos            = build_string + written_length;
        }

        if (*fmtstr != '%') {
            *writing_pos    = *fmtstr;
            fmtstr         += 1;
            writing_pos    += 1;
            written_length += 1;
        } else {
            char const * add_string  = NULL;
            uint64_t addlen          = 0;
            bool addstring_allocated = false;
            switch (fmttypes[fmtcount++]) {
            case (FMT_FILE):
            {
                addlen     = mi.filelen;
                add_string = mi.file;
                break;
            }
            case (FMT_MTHD):
            {
                addlen     = mi.mthdlen;
                add_string = mi.mthd;
                break;
            }
            case (FMT_LINE):
            {
                addlen     = (uint64_t) snprintf(writebuffer, 20, "%d", mi.line);
                add_string = writebuffer;
                break;
            }
            case (FMT_PID):
            {
                addlen     = (uint64_t) snprintf(writebuffer, 20, "%d", (int) mi.pid);
                add_string = writebuffer;
                break;
            }
            case (FMT_TIME):
            {
#define DFLT_TIMESTR "[no time]"
                if (hastimehook) {
                    char * timebuff = NULL;
                    size_t len      = 0;
                    fmt->timehook(&timebuff, &len, mi.ts);
                    if (unlikely(NULL == timebuff)) {
                        addlen     = sizeof(DFLT_TIMESTR) - 1;
                        add_string = writebuffer;
                        memcpy(writebuffer, DFLT_TIMESTR, addlen);
                    } else {
                        addlen              = len;
                        add_string          = timebuff;
                        addstring_allocated = true;
                    }
                } else {
                    addlen     = sizeof(DFLT_TIMESTR) - 1;
                    add_string = writebuffer;
                    memcpy(writebuffer, DFLT_TIMESTR, addlen);
                }
                break;
#undef DFLT_TIMESTR
            }
            case (FMT_STREAMNAME):
            {
                addlen     = strlen(streamname);
                add_string = streamname;
                break;
            }
            case (FMT_SEVERITY):
            {
                char const * const levelstr = scrb_getlevelstr(severity);
                addlen     = strlen(levelstr);
                add_string = levelstr;
                break;
            }
            case (FMT_MSG):
            {
                addlen     = strlen(msg);
                add_string = msg;
                break;
            }
            case (FMT_PRCNT):
            {
                addlen         =  1;
                add_string     = writebuffer;
                writebuffer[0] = '%';
                break;
            }
            default:
                break;
        }
            checklen(&build_string, printbuff, &build_string_allocated,
                     &build_string_cap, &writing_pos, &written_length, addlen);
            memcpy(writing_pos, add_string, addlen);
            writing_pos    += addlen;
            written_length += addlen;
            if (addstring_allocated) {
                free((void *)add_string);
            }
            fmtstr += 2;        // skips over format identifier, as we don't want to print it.
        }
    }

    if (newline) {
        checklen(&build_string, printbuff, &build_string_allocated, &build_string_cap,
                 &writing_pos, &written_length, 1);
        *writing_pos    = '\n';
        writing_pos    += 1;
        written_length += 1;
    }

    *writing_pos  = '\0'; // clean off the end of the buffer before it's written to the output stream
    *total_length = written_length;
    return build_string;
error:
    return NULL;
}

#undef BUILDER_INIT_SIZE

