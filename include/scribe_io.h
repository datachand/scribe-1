//
// IO operations for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_IO_H
#define SCRIBE_IO_H

#include <stdarg.h>
#include <stdbool.h>

#include "scribe_format.h"
#include "scribe_stream.h"
#include "scribe_utils.h"

#ifdef __cplusplus
extern "C" {
#endif

extern
int scrb_putstr_internal(struct scrb_stream * const st, char const * const msg, bool const newline);

extern
int scrb_log_internal(struct scrb_meta_info const mi, 
                       struct scrb_stream * const st,
			           struct scrb_format const * const fmt, 
                       uint16_t const severity,
                       char const * const msg, 
                       bool const newline);

extern
int scrb_flog_internal(struct scrb_meta_info const mi, 
                        struct scrb_stream * const st,
                        struct scrb_format const * const fmt,
                        uint16_t const severity, 
                        char const * const msgfmt, 
                        bool const newline, 
                        va_list ap);

#ifdef __cplusplus
}
#endif

#endif

