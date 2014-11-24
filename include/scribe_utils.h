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
#include <stdlib.h>
#ifdef SCRIBE_WINDOWS
#include <process.h>
#include <windows.h>
#else
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include "scribe_format.h"

struct scrb_meta_info {
    char const * const file;
    char const * const mthd;
    int const line;
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

#define get_meta_info() (struct scrb_meta_info)                                 \
                    { .file = __FILE__, .mthd = __FUNCTION__, .line = __LINE__, \
                      .pid = scrb_getpid(), .ts = scrb_gettime() }

static inline
char * stringdup(char const * const str)
{
    uint64_t const len = strlen(str);
    char * const new = malloc(len + 1);
    if (NULL == new) {
        errno = ENOMEM;
        return NULL;
    }
    memcpy(new, str, len);
    new[len] = '\0';
    return new;
}

static inline
char const * charindex(char const * haystack, char const needle)
{
    while (*haystack != '\0' && *haystack != needle) { haystack += 1; }
    return *haystack == '\0' ? (needle == '\0' ? haystack : NULL) : haystack;
}

extern
char * scrb_build_msg(struct scrb_meta_info const mi, scrb_format const * const fmt, char * const printbuff, 
                      uint64_t const cap, char const * const msg, bool const newline);

#ifdef __cplusplus
}
#endif

#endif

