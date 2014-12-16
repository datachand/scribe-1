//
// Utility functions for internal usage.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_UTILS_H
#define SCRIBE_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "scribe_conf.h"

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef SCRIBE_WINDOWS
#include "Windows.h"
#include <process.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "scribe_format.h"
#include "scribe_types.h"

struct scrb_meta_info {
    char const * const streamname;
    char const * const file;
    char const * const mthd;
    int const line;
    uint64_t const filelen;
    uint64_t const mthdlen;
    SCRIBE_PID_T pid;
    SCRIBE_TIME_T ts;
};

static inline
SCRIBE_TIME_T scrb_gettime(void)
{
#if defined(SCRIBE_WINDOWS)
    SYSTEMTIME ts;
    GetSystemTime(&ts);
#else
    struct timeval ts;
    gettimeofday(&ts, NULL);
#endif
    return ts;
}

static inline
SCRIBE_PID_T scrb_getpid(void)
{
#ifdef SCRIBE_WINDOWS
    return _getpid();
#else
    return getpid();
#endif
}

#define get_meta_info(stname) (struct scrb_meta_info) {         \
                        .streamname = (stname),                 \
                        .file = __FILE__,                       \
                        .mthd = __FUNCTION__,                   \
                        .line = __LINE__,                       \
                        .filelen = sizeof((__FILE__)) - 1,      \
                        .mthdlen = sizeof((__FUNCTION__)) - 1 , \
                        .pid = scrb_getpid(),                   \
                        .ts = scrb_gettime() }

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
char * scrb_build_msg(struct scrb_meta_info const mi, 
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

