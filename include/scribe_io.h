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

#include "scribe_format.h"
#include "scribe_stream.h"

extern
int scrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
			             struct scrb_format const * const fmt, char const * const msg);

extern
int fscrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
                          struct scrb_format const * const fmt, char const * const msgfmt, va_list ap);

#endif

