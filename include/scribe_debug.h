//
// Debug write method implementation
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_DEBUG_H
#define SCRIBE_DEBUG_H

#if !defined(NDEBUG)

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "scribe_conf.h"
#include "scribe_metainfo.h"
#include "scribe_stream.h"

#ifdef __cplusplus
extern "C" {
#endif

static inline
void (scrb_debug_write)(struct scrb_meta_info const mi, char const * const msg, ...)
{
    va_list ap;
    va_start (ap, msg);
    char timebuff[25];
#if defined(SCRIBE_WINDOWS)
    snprinf(timebuff, 25, "%04d:%02d:%02d:%02d:%02d:%02d.%03d",
            mi.ts.wYear, mi.ts.wMonth, mi.ts.wDay, mi.ts.wHour, mi.ts.wMinute, mi.ts.wSecond, mi.ts.Milliseconds);
#else
    strftime(timebuff, 25, "%G:%m:%d:%H:%M:%S.", localtime(&(mi.ts.tv_sec)));
    snprintf(timebuff + 20, 4, "%ld", (long int) mi.ts.tv_usec);
#endif 
    fprintf(scrb_dbg_default.stream.filestream, "%s (%s::%s::%d) ", timebuff, mi.file, mi.mthd, mi.line);
    vfprintf(scrb_dbg_default.stream.filestream, msg, ap);
    fputc('\n', scrb_dbg_default.stream.filestream);
    va_end (ap);
}
#define scrb_debug_write(msg, ...) (scrb_debug_write)(get_meta_info(), (msg), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

#endif

