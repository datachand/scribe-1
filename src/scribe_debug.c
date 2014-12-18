//
// Scribe debug methods implemenation
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include "scribe_conf.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#if defined(SCRIBE_WINDOWS)
#include "Windows.h"
#else
#include <sys/time.h>
#endif

#include "scribe_debug.h"
#include "scribe_metainfo.h"
#include "scribe_stream.h"

#if SCRIBE_DEBUG
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
    fprintf(scrb_debug->stream.filestream, "%s (%s::%s::%d) ", timebuff, mi.file, mi.mthd, mi.line);
    vfprintf(scrb_debug->stream.filestream, msg, ap);
    fputc('\n', scrb_debug->stream.filestream);
    va_end (ap);
}
#else
int unused_dummyvar = 0;
#endif

