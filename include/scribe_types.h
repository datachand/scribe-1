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
    LVLDEBUG  = 0,
    LVLTRACE  = 1,
    LVLINFO   = 2,
    LVLNOTICE = 3,
    LVLWARN   = 4,
    LVLALERT  = 5,
    LVLERROR  = 6,
    LVLPANIC  = 7
};

#ifdef __cplusplus
}
#endif

#endif

