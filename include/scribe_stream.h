// 
// stream implementation for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_STREAM_H
#define SCRIBE_STREAM_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "scribe_conf.h"
#include "scribe_stream.h"
#include "scribe_types.h"

typedef struct scrb_stream scrb_stream;

extern
scrb_stream const stream_out_default;

extern
scrb_stream const stream_in_default;

extern
scrb_stream const stream_err_default;

extern inline
char const * scrb_stream_name__internal(scrb_stream const * const st);

extern
scrb_stream * scrb_open_stream__internal(char const * const path, char const * const mode, bool const synchronize);

extern
void scrb_close_stream__internal(scrb_stream ** streamptr);

extern
int scrb_flush_stream__internal(scrb_stream * const st);

extern
int scrb_purge_stream__internal(scrb_stream * const st);

#endif

