// 
// Format data type for internal usage.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_FORMAT_H
#define SCRIBE_FORMAT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "scribe_conf.h"

typedef enum {
    FMT_UNKOWN = -1,
    FMT_FILE = 0,
    FMT_MTHD = 1,
    FMT_LINE = 2,
    FMT_PID = 3,
    FMT_TIME = 4,
    FMT_STREAMNAME = 5,
    FMT_MSG = 6,
    FMT_PRCNT = 7
} fmttype;

static
char const fmtflags[] = { 'F', 'M', 'L', 'p', 't', 'n', 'm', '%' };

static
uint64_t const num_fmtflags = sizeof(fmtflags) / sizeof(fmtflags[0]);

struct scrb_format {
    char const * fmtstr;
    fmttype const * fmttypes;
    uint64_t numfmts;
};

typedef struct scrb_format scrb_format;

extern
scrb_format * scrb_create_format__internal(char const * const fmtstr);

extern
void scrb_format_release__internal(scrb_format ** fmtptr);

#ifdef __cplusplus
}
#endif

#endif

