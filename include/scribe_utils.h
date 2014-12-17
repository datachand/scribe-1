//
// Utility functions for internal usage.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_UTILS_H
#define SCRIBE_UTILS_H

#include "scribe_conf.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#if defined(SCRIBE_WINDOWS)
#include "Windows.h"
#include <process.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "scribe_format.h"
#include "scribe_metainfo.h"
#include "scribe_return_types.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline
char * stringdup(char const * const str)
{
    uint64_t const len = strlen(str);
    char * const new   = malloc(len + 1);
    if (NULL == new) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(new, str, len);
    new[len] = '\0';
    return new;
}

extern
char * scrb_build_msg(uint16_t const severity,
                      struct scrb_meta_info const mi,
                      struct scrb_format const * const fmt, 
                      char * const printbuff, 
                      uint64_t const cap, 
                      char const * const msg, 
                      uint64_t * const total_length,
                      bool const newline);

#ifdef __cplusplus
}
#endif

#endif

