//
// IO operations for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_IO_H
#define SCRIBE_IO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdbool.h>

#include "scribe.h"
#include "scribe_conf.h"
#include "scribe_format.h"
#include "scribe_stream.h"
#include "scribe_utils.h"

extern
int scrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
			             struct scrb_format const * const fmt, char const * const msg, bool const newline);

extern
int fscrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
                          struct scrb_format const * const fmt, char const * const msgfmt, 
                          bool const newline, va_list ap);

#ifdef __cplusplus
}
#endif

#endif

