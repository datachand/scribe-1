//
// Format data type for internal usage.
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

#include "scribe.h"
#include "scribe_debug.h"
#include "scribe_format.h"
#include "scribe_utils.h"

scrb_format * scrb_create_format__internal(char const * const fmtstr, void (*timehook)(char ** buff, size_t * len, SCRIBE_TIME_T ts))
{
    if (NULL == fmtstr) {
#if SCRIBE_DEBUG
        scrb_debug_write("NULL format string.");
#endif
        goto error;
    }
{
    struct {
        char const * fmtstr;
        void (*timehook)(char ** buff, size_t * maxlen, SCRIBE_TIME_T ts);
        fmttype * fmttypes;
        uint64_t numfmts;
    } tmp;

    tmp.fmtstr = stringdup(fmtstr);
    if(NULL == tmp.fmtstr) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to duplicate format string.");
#endif
        goto error;
    }

    tmp.timehook = timehook;
    tmp.numfmts = 0;
    uint64_t const fl = strlen(tmp.fmtstr);

    // using trivial upper bound on how many possible
    // formatters exist in the string.
    fmttype tmp_fmttypes[(fl / 2) + 1];
    
    char const * curpos = tmp.fmtstr; 
    while (*curpos != '\0') {
        if (*curpos != '%') {
            curpos += 1;
            continue;
        } else {
            char const fmtchar = *(curpos + 1);
            if (fmtchar == fmtflags[FMT_FILE]) {
                tmp_fmttypes[tmp.numfmts] = FMT_FILE;
            } else if (fmtchar == fmtflags[FMT_MTHD]) {
                tmp_fmttypes[tmp.numfmts] = FMT_MTHD;
            } else if (fmtchar == fmtflags[FMT_LINE]) {
                tmp_fmttypes[tmp.numfmts] = FMT_LINE;
            } else if (fmtchar == fmtflags[FMT_PID]) {
                tmp_fmttypes[tmp.numfmts] = FMT_PID;
            } else if (fmtchar == fmtflags[FMT_TIME]) {
                tmp_fmttypes[tmp.numfmts] = FMT_TIME;
            } else if (fmtchar == fmtflags[FMT_STREAMNAME]) {
                tmp_fmttypes[tmp.numfmts] = FMT_STREAMNAME;
            } else if (fmtchar == fmtflags[FMT_MSG]) {
                tmp_fmttypes[tmp.numfmts] = FMT_MSG;
            } else if (fmtchar == fmtflags[FMT_PRCNT]) {
                tmp_fmttypes[tmp.numfmts] = FMT_PRCNT;
            } else {
#if SCRIBE_DEBUG
                scrb_debug_write("Unrecongnized format character '%c'", fmtchar);
#endif
                goto error;
            }
            tmp.numfmts += 1;
            curpos += 1;
        }
    }
    
    // Now allocate for actual tmp split forms
    tmp.fmttypes = malloc(tmp.numfmts * sizeof(fmttype));
    memcpy(tmp.fmttypes, tmp_fmttypes, tmp.numfmts * sizeof(fmttype));

    struct scrb_format * result = malloc(sizeof(struct scrb_format));
    memcpy(result, &tmp, sizeof(struct scrb_format));
    return result;
}
error:
    return NULL;
}

void scrb_format_release__internal(scrb_format ** fmtptr)
{
    if (NULL != fmtptr && NULL != *fmtptr) {
        scrb_format * fmt = *fmtptr;
        free((void *)fmt->fmtstr);
        free((void *)fmt->fmttypes);
        free(fmt);
    }
}

