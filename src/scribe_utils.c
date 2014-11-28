// 
// internal utility functions for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include "scribe_conf.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scribe_debug.h"
#include "scribe_format.h"
#include "scribe_types.h"
#include "scribe_utils.h"

#define BUILDER_INIT_SIZE 128

static inline
char * reallocstr(char * str, bool const allocated, uint64_t const newcap)
{
    return (allocated ? realloc(str, newcap * sizeof(char)) : malloc(newcap * sizeof(char)));
}

char * scrb_build_msg(struct scrb_meta_info const mi, scrb_format const * const fmt, char * const printbuff, 
                      uint64_t const cap, char const * const msg, bool const newline)
{
    bool allocated = false;
    char * wrtstr = printbuff;
    char * wrtpos = wrtstr;
    uint64_t wrtcap = cap;
    char const * fmtstr = fmt->fmtstr;
    uint64_t const numfmts = fmt->numfmts;

    // we'll copy the format types into a static array to remove one level
    // of pointer indirection to pass through during the loop below.
    fmttype const fmttypes[numfmts];
    memcpy((void *)fmttypes, fmt->fmttypes, numfmts * sizeof(fmttype));

    uint64_t wrtlen = 0, fmtcount = 0;
    while (*fmtstr != '\0') {  
        // oops, looks like we've hit the write capacity,
        // we'll allocate a new buffer and work from there.
        if (unlikely(wrtlen == wrtcap)) {
            wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
            if (NULL == wrtstr) {
                goto error;
            }
            if (!allocated) {
                memcpy(wrtstr, printbuff, wrtlen);
            }
            allocated = true;
            wrtcap *= 2;
            wrtpos = wrtstr + wrtlen;
        }

        if (*fmtstr != '%') {
            *wrtpos = *fmtstr;
            fmtstr += 1;
            wrtpos += 1;
            wrtlen += 1;
        } else {
            uint64_t addlen = 0;
            switch (fmttypes[fmtcount++]) {
                case (FMT_FILE):
                {
                    addlen = strlen(mi.file);
                    if (unlikely(wrtlen + addlen >= wrtcap)) {
                        wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
                        if (NULL == wrtstr) {
                            goto error;
                        }
                        if (!allocated) {
                            memcpy(wrtstr, printbuff, wrtlen);
                        }
                        allocated = true;
                        wrtcap *= 2;
                        wrtpos = wrtstr + wrtlen;
                    } 
                    memcpy(wrtpos, mi.file, addlen);
                    break;
                }
                case (FMT_MTHD):
                {
                    addlen = strlen(mi.mthd);
                    if (unlikely(wrtlen + addlen >= wrtcap)) {
                        wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
                        if (NULL == wrtstr) {
                            goto error;
                        }
                        if (!allocated) {
                            memcpy(wrtstr, printbuff, wrtlen);
                        }
                        allocated = true;
                        wrtcap *= 2;
                        wrtpos = wrtstr + wrtlen;
                    } 
                    memcpy(wrtpos, mi.mthd, addlen);
                    break;
                }
                case (FMT_LINE):
                {
                    char numstr[20];
                    addlen = snprintf(numstr, 20, "%d", mi.line);
                    if (unlikely(wrtlen + addlen >= wrtcap)) {
                        wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
                        if (NULL == wrtstr) {
                            goto error;
                        }
                        if (!allocated) {
                            memcpy(wrtstr, printbuff, wrtlen);
                        }
                        allocated = true;
                        wrtcap *= 2;
                        wrtpos = wrtstr + wrtlen;
                    } 
                    memcpy(wrtpos, numstr, addlen);
                    break;
                }
                case (FMT_PID):
                {
                    char numstr[20];
                    addlen = snprintf(numstr, 20, "%d", (int) mi.pid);
                    if (unlikely(wrtlen + addlen >= wrtcap)) {
                        wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
                        if (NULL == wrtstr) {
                            goto error;
                        }
                        if (!allocated) {
                            memcpy(wrtstr, printbuff, wrtlen);
                        }
                        allocated = true;
                        wrtcap *= 2;
                        wrtpos = wrtstr + wrtlen;
                    } 
                    memcpy(wrtpos, numstr, addlen);
                    break;
                }
                // TODO: incorporate some sort of time formatting option
                /*
                case (FMT_TIME):
                {
                    if (SCRIBE_Success != string_builder_write_str(&builder, "notime", strlen("notime"))) {
                        if (NULL != builder.str) {
                            free(builder.str);
                        }
                        goto error;
                    }
                    break;
                }
                */
                case (FMT_STREAMNAME):
                {
                    addlen = strlen(mi.streamname);
                    if (unlikely(wrtlen + addlen >= wrtcap)) {
                        wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
                        if (NULL == wrtstr) {
                            goto error;
                        }
                        if (!allocated) {
                            memcpy(wrtstr, printbuff, wrtlen);
                        }
                        allocated = true;
                        wrtcap *= 2;
                        wrtpos = wrtstr + wrtlen;
                    }
                    memcpy(wrtpos, mi.streamname, addlen);
                    break;
                }
                case (FMT_MSG):
                {
                    addlen = strlen(msg);
                    if (unlikely(wrtlen + addlen >= wrtcap)) {
                        wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
                        if (NULL == wrtstr) {
                            goto error;
                        }
                        if (!allocated) {
                            memcpy(wrtstr, printbuff, wrtlen);
                        }
                        allocated = true;
                        wrtcap *= 2;
                        wrtpos = wrtstr + wrtlen;
                    } 
                    memcpy(wrtpos, msg, addlen);
                    break;
                }
                case (FMT_PRCNT):
                {
                    addlen = 1;
                    if (unlikely(wrtlen + addlen >= wrtcap)) {
                        wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
                        if (NULL == wrtstr) {
                            goto error;
                        }
                        if (!allocated) {
                            memcpy(wrtstr, printbuff, wrtlen);
                        }
                        allocated = true;
                        wrtcap *= 2;
                        wrtpos = wrtstr + wrtlen;
                    }
                    *wrtpos = '%';
                    break;
                }
                default:
                    break;
            }
            wrtlen += addlen;
            wrtpos += addlen;
            fmtstr += 2;        // skips over format identifier, as we don't want to print it.
        }
    }

    if (newline) {
        if (unlikely(wrtlen >= wrtcap - 1)) {
            wrtstr = reallocstr(wrtstr, allocated, 2 * wrtcap);
            if (NULL == wrtstr) {
                goto error; 
            }
            if (!allocated) {
                memcpy(wrtstr, printbuff, wrtlen);
            }
            wrtpos = wrtstr + wrtlen;
        }
        *wrtpos = '\n';
        wrtpos += 1;
        wrtlen += 1; 
    }

    *wrtpos = '\0'; // clean off the end of the buffer before it's written to the output stream
    return wrtstr;
error:
    return NULL;
}

#undef BUILDER_INIT_SIZE

