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

enum scrb_format_type {
    FMT_UNKOWN = -1,
    FMT_FILE,
    FMT_MTHD,
    FMT_LINE,
    FMT_PID,
    FMT_TIME,
    FMT_MSG
};

struct split_form {
    enum scrb_format_type const type;
    char const * const string;
    uint64_t const len;
};

struct scrb_format {
    char const * const fmtstr;
    uint64_t const fmtstr_len;
    struct split_form const * const fmtstr_split;
    uint64_t const nsplits;
};

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
            splitpos = startpoint + i + 2; // skip over formatter.
            break;
        } 
    }

    while (i < maxlen - 1 && startpoint[i] != '%') { i += 1; len += 1; }

    return (struct split_form) { .type = type, .string = len != 0 ? strndup(splitpos, len) : NULL, .len = len };
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
    uint64_t l = tmp.fmtstr_len;
    uint64_t i;
    for (i = 0; i < l; i += 1) {
        struct split_form parsed = parse_formatter(curpos, l - i);
        if (FMT_UNKOWN == parsed.type) {
#ifdef SCRIBE_DEBUG
            fprintf(stderr, "unkown format identifier");
#endif
            goto error;
        } else {
            tmp_split_forms[tmp.nsplits] = parsed;
            tmp.nsplits += 1;
        }
    }

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

void scrb_format_release__internal(struct scrb_format ** fmtptr)
{
    if (NULL != fmtptr && NULL != *fmtptr) {
        free((void *)(*fmtptr)->fmtstr);
        free((void *)(*fmtptr)->fmtstr_split);
        free(*fmtptr);     
    }
}

