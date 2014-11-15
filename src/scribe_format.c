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

#include "scribe_format.h"

static inline
struct split_form parse_formatter(char const * const startpoint, uint64_t const maxlen)
{
    enum scrb_format_type type;
    char const * splitpos = NULL;
    uint64_t len = 0;

    uint64_t i;
    for (i = 0 ; i < maxlen - 1; i += 1) {
        if ('%' == startpoint[i]) {
            switch (startpoint[i+1]) {
                case ('f'):
                {
                    type = FMT_FILE;
                    break;
                }
                case ('m'):
                {
                    type = FMT_MTHD;
                    break;
                }
                case ('l'):
                {
                    type = FMT_LINE;
                    break;
                }
                case ('p'):
                {
                    type = FMT_PID;
                    break;
                }
                case ('t'):
                {
                    type = FMT_TIME;
                    break;
                }
                case ('M'):
                {
                    type = FMT_MSG;
                    break;
                }
                default:
                {
                    type = FMT_UNKOWN;
                    goto error;
                }
            }
            splitpos = startpoint + i;
            break;
        } 
    }

    do { i += 1; len += 1; } while (i < maxlen && startpoint[i] != '%'); 

    return (struct split_form) { .type = type, .string = strndup(splitpos, len), .len = len };
error:
    return (struct split_form) { .type = type, .string = NULL, .len = 0 };
}

scrb_format * scrb_create_format__internal(char const * const fmtstr)
{
    if (NULL == fmtstr) {
        goto error;
    }
{
    struct {
        char const * fmtstr;
        uint64_t fmtstr_len;
        char const * fmtleader;
        struct split_form * fmtstr_split_forms;
        uint64_t nsplits;
    } tmp;
    
    tmp.fmtstr = strdup(fmtstr);
    tmp.fmtstr_len = strlen(fmtstr);
    tmp.nsplits = 0;

    // using trivial upper bound on how many possible
    // split forms exist in the string.
    struct split_form tmp_split_forms[(tmp.fmtstr_len / 2) + 1];

    char const * curpos = tmp.fmtstr;
    uint64_t fmtlen = tmp.fmtstr_len;
    uint64_t totlen = 0;
    
    while (*curpos != '%' && totlen < fmtlen) { curpos += 1; totlen += 1; }
    tmp.fmtleader = strndup(tmp.fmtstr, totlen);

    do {
        struct split_form parsed = parse_formatter(curpos, fmtlen - totlen);
        if (FMT_UNKOWN == parsed.type) {
#ifdef SCRIBE_DEBUG
            fprintf(stderr, "unkown format identifier\n");
#endif
            free((void *)tmp.fmtstr);
            goto error;            
        }
        tmp_split_forms[tmp.nsplits] = parsed;
        tmp.nsplits += 1;
        curpos += parsed.len;
        totlen += parsed.len;
    } while(totlen < fmtlen);
    
    // now allocate for actual tmp split forms
    tmp.fmtstr_split_forms = malloc(tmp.nsplits * sizeof(struct split_form));
    memcpy(tmp.fmtstr_split_forms, tmp_split_forms, tmp.nsplits * sizeof(struct split_form));

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
        free((void *)fmt->fmtleader);
        uint64_t i;
        for (i = 0; i < fmt->nsplits; i += 1) {
            free((void *)fmt->fmtstr_split[i].string);
        }
        free((void *)fmt->fmtstr_split);
        free(fmt);
    }
}

