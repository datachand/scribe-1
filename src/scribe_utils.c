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

#include "scribe_format.h"
#include "scribe_types.h"
#include "scribe_utils.h"

#define BUILDER_INIT_SIZE 256

struct string_builder {
    char * str;
    uint64_t len;
    uint64_t cap;
};

static inline
void string_builder_expand(struct string_builder * const b)
{
    realloc(b->str, b->cap * 2);
    b->cap *= 2;
}

static
int string_builder_write_str(struct string_builder * const b, char const * const wstr, uint64_t const wstrlen)
{
    if (0 == wstrlen) {
        return (SCRIBE_Success);
    }
 #ifdef SCRIBE_DEBUG
    if (NULL == b) {
        fprintf(stderr, "NULL string_builder pointer.\n");
        goto error;
    } else if (NULL == wstr) {
        fprintf(stderr, "NULL string passed.\n");
        goto error;
    }
#endif
    while (b->len + wstrlen >= b->cap - 1) {
        string_builder_expand(b);
        if (NULL == b->str) {
            goto error;
        }
    }
    
    memcpy(b->str + b->len, wstr, wstrlen);
    b->len += wstrlen;
    return (SCRIBE_Success);
error:
    return (SCRIBE_Failure);
}

char * scrb_build_msg(struct scrb_meta_info const mi, scrb_format const * const fmt,
                      char const * const msg, bool const newline)
{
    struct string_builder builder;
    builder.str = malloc(BUILDER_INIT_SIZE * sizeof(char));
    builder.len = 0;
    builder.cap = BUILDER_INIT_SIZE;
    
    if (NULL == builder.str) {
#if SCRIBE_DEBUG
        fprintf(stderr, "Failed to build message string.\n");
#endif
        goto error;
    }

    string_builder_write_str(&builder, fmt->fmtleader, strlen(fmt->fmtleader));

    uint64_t i;
    for (i = 0; i < fmt->nsplits; i += 1) {  
        switch (fmt->fmtstr_split[i].type) {
            case (FMT_FILE):
            {
                if (SCRIBE_Success != string_builder_write_str(&builder, mi.file, strlen(mi.file))) {
                    if (NULL != builder.str) {
                        free(builder.str);
                    }
                    goto error;
                }
                break;
            }
            case (FMT_MTHD):
            {
                if (SCRIBE_Success != string_builder_write_str(&builder, mi.mthd, strlen(mi.mthd))) {
                    if (NULL != builder.str) {
                        free(builder.str);
                    }
                    goto error;
                }
                break;
            }
            case (FMT_LINE):
            {
                char ls[20];
                uint64_t const len = snprintf(ls, 20, "%d", mi.line);
                if (SCRIBE_Success != string_builder_write_str(&builder, ls, len)) {
                    if (NULL != builder.str) {
                        free(builder.str);
                    }
                    goto error;
                }
                break;
            }
            case (FMT_PID):
            {
                char ls[20];
                uint64_t const len = snprintf(ls, 20, "%d", (int) mi.pid);
                if (SCRIBE_Success != string_builder_write_str(&builder, ls, len)) {
                    if (NULL != builder.str) {
                        free(builder.str);
                    }
                    goto error;
                }
                break;
            }
            case (FMT_TIME):
            {
                // TODO: incorporate some sort of time formatting option
                if (SCRIBE_Success != string_builder_write_str(&builder, "notime", strlen("notime"))) {
                    if (NULL != builder.str) {
                        free(builder.str);
                    }
                    goto error;
                }
                break;
            }
            case (FMT_MSG):
            {
                if (SCRIBE_Success != string_builder_write_str(&builder, msg, strlen(msg))) {
                    if (NULL != builder.str) {
                        free(builder.str);
                    }
                    goto error;
                }
                break;
            }
            default: break;
        }
        if (SCRIBE_Success != string_builder_write_str(&builder, 
                                                       fmt->fmtstr_split[i].string + 2, 
                                                       fmt->fmtstr_split[i].len - 2))
        {
            if (NULL != builder.str) {
                free(builder.str);
            }
            goto error;    
        }
    }

    if (newline) {
        if (SCRIBE_Success != string_builder_write_str(&builder, "\n", 1)) {
            if (NULL != builder.str) {
                free(builder.str);
            }
            goto error;
        }
    }
    char * ret = malloc((builder.len + 1) * sizeof(char));
    memcpy(ret, builder.str, builder.len);
    free(builder.str);
    return ret;
error:
    return NULL;
}

#undef BUILDER_INIT_SIZE

