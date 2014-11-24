//
// Header file & api for scribe library
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_H
#define SCRIBE_H

#ifdef __cplusplus
extern "C" {
#endif

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
#include "spinlock.h"

static
scrb_stream const * const scrb_stdout = &stream_out_default;

static
scrb_stream const * const scrb_stdin = &stream_in_default;

static
scrb_stream const * const scrb_stderr = &stream_err_default;

#if SCRIBE_DEBUG
static
scrb_stream const * const scrb_debug = &scrb_dbg_default;
#endif

// `scrb_init`
// doc...
static inline
int scrb_init(void)
{
    scrb_stream const outstream = (scrb_stream) {
	    .name = "stdout",
	    .readable = false,
	    .writeable = true,
	    .synchronize = true,
        .memmap = false,
        .stream = {
            .filestream = stdout
        },
	    .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
    };

    scrb_stream const instream = (scrb_stream) {
	    .name = "stdin",
	    .readable = true,
	    .writeable = false,
	    .synchronize = true,
        .memmap = false,
        .stream = {
            .filestream = stdin
        },
        .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
    };

    scrb_stream const errstream = (scrb_stream) {
	    .name = "stderr",
	    .readable = false,
	    .writeable = true,
	    .synchronize = true,
        .memmap = false,
        .stream = {
            .filestream = stderr
        },
        .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY) 
    };

#if SCRIBE_DEBUG
    scrb_stream const dbgstream = (scrb_stream) {
        .name = "debug",
        .readable = false,
        .writeable = true,
        .synchronize = true,
        .memmap = false,
        .stream = {
            .filestream = fopen("scribedebug.log", "a")
        },
        .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
    };

    if (NULL == dbgstream.stream.filestream) {
        return (SCRIBE_Failure);
    }

    scrb_init_defaults(&outstream, &instream, &errstream, &dbgstream);
    fprintf(dbgstream.stream.filestream, "----- scribe init :: version (%s) :: compiled (%s %s) :: pid (%d) -----\n",
            SCRIBE_VERSION, __DATE__, __TIME__, (int) scrb_getpid());
#else
    scrb_init_defaults(&outstream, &instream, &errstream);
#endif
    return (SCRIBE_Success);
}

// `scrb_create_format`
// doc...
static inline
scrb_format * scrb_create_format(char const * const fmtstr)
{
    return scrb_create_format__internal(fmtstr);
}

//
//
static inline
void (scrb_format_release)(scrb_format ** fmtptr)
{
    scrb_format_release__internal(fmtptr);
}
#define scrb_format_release(fmtptr) (scrb_format_release)((scrb_format **) (fmtptr))

// `scrb_stream_name`
// doc...
static inline
char const * scrb_stream_name(scrb_stream const * const st)
{
    return scrb_stream_name__internal(st);
}

// `scribe_open_file`
// doc...
static inline
scrb_stream * scrb_open_stream(char const * const path, char const * const mode,
                               bool const synchronize, bool const memmap)
{
    return scrb_open_stream__internal(path, mode, synchronize, memmap);
}

// `scribe_close`
// doc...
static inline
void (scrb_close_stream)(scrb_stream ** streamptr)
{
    scrb_close_stream__internal(streamptr);
}
#define scrb_close_stream(streamptr) (scrb_close_stream)((scrb_stream **)(streamptr))

// `scribe_flush_stream`
// doc...
static inline
void scrb_flush_stream(scrb_stream * const st)
{
    scrb_flush_stream__internal(st);
}

// `scribe_write`
// doc...
static inline
int (scrb_write)(struct scrb_meta_info const mi, scrb_stream const * const st, 
                 struct scrb_format const * const fmt, char const * const msg)
{
    return scrb_write__internal(mi, st, fmt, msg, false); 
}
#define scrb_write(st, fmt, msg) (scrb_write)(get_meta_info(), (st), (fmt), (msg))

// `scribe_writeln`
// doc...
static inline
int (scrb_writeln)(struct scrb_meta_info const mi, scrb_stream const * const st, 
                   struct scrb_format const * const fmt, char const * const msg)
{
    return scrb_write__internal(mi, st, fmt, msg, true); 
}
#define scrb_writeln(st, fmt, msg) (scrb_writeln)(get_meta_info(), (st), (fmt), (msg))

// `fscribe_write`
// doc...
static inline
int (fscrb_write)(struct scrb_meta_info const mi, scrb_stream const * const st,
                  struct scrb_format const * const fmt, char const * const msgfmt, ...)
{
    va_list ap;
    va_start (ap, msgfmt);
    int const rval = fscrb_write__internal(mi, st, fmt, msgfmt, false, ap);
    va_end (ap);
    return rval;
}
#define fscrb_write(st, fmt, msg, ...) (fscrb_write)(get_meta_info(), (st), (fmt), (msg), ##__VA_ARGS__)

// `fscribe_writeln`
// doc...
static inline
int (fscrb_writeln)(struct scrb_meta_info const mi, scrb_stream const * const st,
                    struct scrb_format const * const fmt, char const * const msgfmt, ...)
{
    va_list ap;
    va_start (ap, msgfmt);
    int const rval = fscrb_write__internal(mi, st, fmt, msgfmt, true, ap);
    va_end (ap);
    return rval;
}
#define fscrb_writeln(st, fmt, msg, ...) (fscrb_writeln)(get_meta_info(), (st), (fmt), (msg), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

