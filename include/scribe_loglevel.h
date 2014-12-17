//
// Definition of log severity levels and corresponding strings
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_LOGLEVEL_H
#define SCRIBE_LOGLEVEL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

enum scrb_loglevel {
    LVL_DEBUG  = (1 << 0),
    LVL_TRACE  = (1 << 1),
    LVL_INFO   = (1 << 2),
    LVL_NOTICE = (1 << 3),
    LVL_WARN   = (1 << 4),
    LVL_ALERT  = (1 << 5),
    LVL_ERROR  = (1 << 6),
    LVL_EMERG  = (1 << 7)
};

static char const * levelstrs[] = { "DEBUG", "TRACE", "INFO", "NOTICE", "WARN", "ALERT", "ERROR", "EMERG", "unknown" };

static inline
char const * scrb_getlevelstr(uint16_t const severity)
{
    return severity & LVL_DEBUG  ? levelstrs[0]
         : severity & LVL_TRACE  ? levelstrs[1]
         : severity & LVL_INFO   ? levelstrs[2]
         : severity & LVL_NOTICE ? levelstrs[3]
         : severity & LVL_WARN   ? levelstrs[4]
         : severity & LVL_ALERT  ? levelstrs[5]
         : severity & LVL_ERROR  ? levelstrs[6]
         : severity & LVL_EMERG  ? levelstrs[7] : levelstrs[8];
}

#ifdef __cplusplus
}
#endif

#endif

