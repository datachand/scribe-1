//
// Definition of log metainfo type and utility methods for acquiring PID and system time.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_METAINFO_H
#define SCRIBE_METAINFO_H

#include "scribe_conf.h"

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

#include "scribe_return_types.h"

#ifdef __cplusplus
extern "C" {
#endif

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
#if defined(SCRIBE_WINDOWS)
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

#ifdef __cplusplus
}
#endif

#endif

