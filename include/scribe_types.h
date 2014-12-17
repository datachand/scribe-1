// 
// Definitions of internal types used in scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_TYPES_H
#define SCRIBE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

enum scrb_return {
	SCRB_Success = 0,
	SCRB_Failure = 1
};

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

static char const * lvlstrings[] = { "DEBUG", "TRACE", "INFO", "NOTICE", "WARN", "ALERT", "ERROR", "EMERG", "uknown" };

#ifdef __cplusplus
}
#endif

#endif

