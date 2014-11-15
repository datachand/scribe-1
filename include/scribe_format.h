// 
// Format data type for internal usage.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_FORMAT_H
#define SCRIBE_FORMAT_H

#include <stdint.h>
#include <stdlib.h>

#include "scribe_conf.h"

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
    char const * const fmtleader;
    struct split_form const * const fmtstr_split;
    uint64_t const nsplits;
};

typedef struct scrb_format scrb_format;

extern
scrb_format * scrb_create_format__internal(char const * const fmtstr);

extern
void scrb_format_release__internal(scrb_format ** fmtptr);

#endif

