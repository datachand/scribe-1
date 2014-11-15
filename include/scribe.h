//
// Header file & api for scribe library
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_H
#define SCRIBE_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scribe_conf.h"
#include "scribe_io.h"
#include "scribe_stream.h"
#include "scribe_types.h"
#include "scribe_format.h"
#include "scribe_utils.h"

scrb_stream const * const scrb_stdout = &stream_out_default;

scrb_stream const * const scrb_stdin = &stream_in_default;

scrb_stream const * const scrb_stderr = &stream_err_default;

// `scrb_create_format`
// doc...
extern inline
scrb_format * scrb_create_format(char const * const fmtstr)
{
    return scrb_create_format__internal(fmtstr);
}

//
//
extern inline
void (scrb_format_release)(scrb_format ** fmtptr)
{
    scrb_format_release__internal(fmtptr);
}
#define scrb_format_release(fmtptr) (scrb_format_release((scrb_format **) (fmtptr))

// `scrb_stream_name`
// doc...
extern inline
char const * scrb_stream_name(scrb_stream const * const st)
{
    return scrb_stream_name__internal(st);
}

// `scribe_open_file`
// doc...
extern inline
scrb_stream * scrb_open_stream(char const * const path, char const * const mode, bool const synchronize)
{
    return scrb_open_stream__internal(path, mode, synchronize);
}

// `scribe_close`
// doc...
extern inline
void scrb_close_stream(scrb_stream ** streamptr)
{
    scrb_close_stream__internal(streamptr);
}

// `scribe_flush_stream`
// doc...
extern inline
int scrb_flush_stream(scrb_stream * const st)
{
    return scrb_flush_stream__internal(st);
}

// `scribe_purge_stream`
// doc...
extern inline
int scrb_purge_stream(scrb_stream * const st)
{
    return scrb_purge_stream__internal(st);
}

// `scribe_write`
// doc...
extern inline
int (scrb_write)(struct scrb_meta_info const mi, scrb_stream const * const st, 
                 struct scrb_fromat const * const fmt, char const * const msg)
{
    return scrb_write__internal(mi, st, fmt, msg); 
}
#define scrb_write(st, fmt, msg) (scribe_log)(get_meta_info(), (st), (fmt), (msg))

// `fscribe_write`
// doc...
extern inline
int (fscribe_write)(struct scrb_meta_info const mi, scrb_stream const * const st,
                    struct scrb_format const * const fmt, char const * const msgfmt, ...)
{
    va_list ap;
    va_start (msgfmt, ap);
    int const rval = fscrb_write__internal(mi, st, fmt, msgfmt, ap);
    va_end (ap);
    return rval;
}
#define fscrb_write(st, fmt, msg, ...) (scribe_log)(get_meta_info(), (st), (fmt), (msg), ##__VA_ARGS__)

#endif

