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

#include "scribe_conf.h"
#include "scribe_stream.h"
#include "scribe_utils.h"

#if SCRIBE_DEBUG
static inline
void (scrb_debug_write)(struct scrb_meta_info const mi, char const * const msg, ...)
{
    va_list ap;
    va_start (ap, msg);
    fprintf(scrb_dbg_default.stream.filestream, "(%s::%s::%d) ", mi.file, mi.mthd, mi.line);
    vfprintf(scrb_dbg_default.stream.filestream, msg, ap);
    fputc('\n', scrb_dbg_default.stream.filestream);
    va_end (ap);
}
#define scrb_debug_write(msg, ...) (scrb_debug_write)(get_meta_info(), (msg), ##__VA_ARGS__)
#endif

#endif

