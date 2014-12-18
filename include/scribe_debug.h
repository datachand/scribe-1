//
// Debug write method implementation
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_DEBUG_H
#define SCRIBE_DEBUG_H

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "scribe_conf.h"
#include "scribe_metainfo.h"

#ifdef __cplusplus
extern "C" {
#endif

#if SCRIBE_DEUBG

extern
struct scrb_stream * scrb_debug;

extern
void (scrb_debug_write)(struct scrb_meta_info const mi, char const * const msg, ...);

#define scrb_debug_write(msg, ...) (scrb_debug_write)(get_meta_info(), (msg), ##__VA_ARGS__)

#else
extern int unused_dummyvar;
#endif

#ifdef __cplusplus
}
#endif

#endif

