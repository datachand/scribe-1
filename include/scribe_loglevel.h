//
// Definition of log severity levels and corresponding strings
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_LOGLEVEL_H
#define SCRIBE_LOGLEVEL_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

enum scrb_loglevel {
    LVL_DEBUG  = 0,
    LVL_TRACE,
    LVL_INFO,
    LVL_NOTICE,
    LVL_WARN,
    LVL_ALERT,
    LVL_ERROR,
    LVL_EMERG
};

static char const * levelstrs[] = { "DEBUG", "TRACE", "INFO", "NOTICE", "WARN", "ALERT", "ERROR", "EMERG", "unknown" };

static inline
char const * scrb_getlevelstr(enum scrb_loglevel const severity)
{
    return severity == LVL_DEBUG  ? levelstrs[0]
         : severity == LVL_TRACE  ? levelstrs[1]
         : severity == LVL_INFO   ? levelstrs[2]
         : severity == LVL_NOTICE ? levelstrs[3]
         : severity == LVL_WARN   ? levelstrs[4]
         : severity == LVL_ALERT  ? levelstrs[5]
         : severity == LVL_ERROR  ? levelstrs[6]
         : severity == LVL_EMERG  ? levelstrs[7] : levelstrs[8];
}

#ifdef __cplusplus
}
#endif

#endif

