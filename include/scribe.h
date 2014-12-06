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
#include "scribe_debug.h"
#include "scribe_io.h"
#include "scribe_stream.h"
#include "scribe_types.h"
#include "scribe_format.h"
#include "scribe_utils.h"
#include "scribe_ringbuff.h"
#include "spinlock.h"

#if defined(SCRIBE_WINDOWS)
#include "Windows.h"
#endif

static
struct scrb_stream * const scrb_stdout = &stream_out_default;

static
struct scrb_stream * const scrb_stdin  = &stream_in_default;

static
struct scrb_stream * const scrb_stderr = &stream_err_default;

#if SCRIBE_DEBUG
static
struct scrb_stream const * const scrb_debug  = &scrb_dbg_default;
#endif

// `scrb_init`
// doc...
static inline
int scrb_init(void)
{
#if defined(SCRIBE_WINDOWS)
    static volatile LONG initialized = 0;
    if (0 == InterlockedCompareExchange(&initialized, 1, 0)) {
#else    
    static volatile int initialized = 0;
    if (__sync_bool_compare_and_swap(&initialized, 0, 1)) {
#endif
        struct scrb_stream outstream = (struct scrb_stream) {
            .name        = "stdout",
            .synchronize = true,
            .stream = {
                .filestream = stdout,
                .filedes    = fileno(stdout),
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        struct scrb_stream instream = (struct scrb_stream) {
            .name        = "stdin",
            .synchronize = true,
            .stream = {
                .filestream = stdin,
                .filedes    = fileno(stdin),
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        struct scrb_stream errstream = (struct scrb_stream) {
            .name        = "stderr",
            .synchronize = true,
            .stream = {
                .filestream = stderr,
                .filedes    = fileno(stderr),
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY) 
        };

#if SCRIBE_DEBUG
        FILE * const dbg_filestream = fopen("scribedebug.log", "a");
        struct scrb_stream dbgstream = (struct scrb_stream) {
            .name        = "debug",
            .synchronize = true,
            .stream = {
                .filestream = dbg_filestream,
                .filedes    = fileno(dbg_filestream),
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        if (NULL == dbgstream.stream.filestream) {
            return (SCRB_Failure);
        }

        scrb_init_defaults(&outstream, &instream, &errstream, &dbgstream);
        fprintf(dbgstream.stream.filestream, "----- scribe init :: version (%s) :: compiled (%s %s) :: pid (%d) -----\n",
                SCRIBE_VERSION, __DATE__, __TIME__, (int) scrb_getpid());
#else
        scrb_init_defaults(&outstream, &instream, &errstream);
#endif
    }
    return (SCRB_Success);
}

// `scrb_create_format`
// doc...
static inline
struct scrb_format * scrb_create_format(char const * const fmtstr, 
                                        void (*timehook)(char ** buff, 
                                                         size_t * len, 
                                                         SCRIBE_TIME_T ts))
{
    return scrb_create_format__internal(fmtstr, timehook);
}

//
//
static inline
void (scrb_format_release)(struct scrb_format ** fmtptr)
{
    scrb_format_release__internal(fmtptr);
}
#define scrb_format_release(fmtptr) (scrb_format_release)((struct scrb_format **) (fmtptr))

// `scrb_stream_name`
// doc...
static inline
char const * scrb_stream_name(struct scrb_stream const * const st)
{
    return scrb_stream_name__internal(st);
}

// `scribe_open_file`
// doc...
static inline
struct scrb_stream * scrb_open_stream(char const * const path, 
                                      char const * const mode, 
                                      bool const synchronize, 
                                      bool const use_buffer)
{
    return scrb_open_stream__internal(path, mode, synchronize, use_buffer);
}

// `scribe_close`
// doc...
static inline
void (scrb_close_stream)(struct scrb_stream ** streamptr)
{
    scrb_close_stream__internal(streamptr);
}
#define scrb_close_stream(streamptr) (scrb_close_stream)((struct scrb_stream **)(streamptr))

// `scribe_flush_stream`
// doc...
static inline
void scrb_flush_stream(struct scrb_stream * const st)
{
    scrb_flush_stream__internal(st);
}

// `scribe_write`
// doc...
static inline
int (scrb_write)(struct scrb_meta_info const mi, 
                 struct scrb_stream * const st, 
                 struct scrb_format const * const fmt, 
                 char const * const msg)
{
    return scrb_write__internal(mi, st, fmt, msg, false); 
}
#define scrb_write(st, fmt, msg) (scrb_write)(get_meta_info((st)->name), (st), (fmt), (msg))

// `scribe_writeln`
// doc...
static inline
int (scrb_writeln)(struct scrb_meta_info const mi, 
                   struct scrb_stream * const st, 
                   struct scrb_format const * const fmt, 
                   char const * const msg)
{
    return scrb_write__internal(mi, st, fmt, msg, true); 
}
#define scrb_writeln(st, fmt, msg) (scrb_writeln)(get_meta_info((st)->name), (st), (fmt), (msg))

// `scribe_writestr`
// doc...
static inline
int scrb_putstr(struct scrb_stream * const st, char const * const msg)
{
    return scrb_putstr__internal(st, msg, false);
}

// `scribe_writestrln`
// doc...
static inline
int scrb_putstrln(struct scrb_stream * const st, char const * const msg)
{
    return scrb_putstr__internal(st, msg, true);
}

// `fscribe_write`
// doc...
static inline
int (fscrb_write)(struct scrb_meta_info const mi, 
                  struct scrb_stream * const st,
                  struct scrb_format const * const fmt, 
                  char const * const msgfmt, ...)
{
    va_list ap;
    va_start (ap, msgfmt);
    int const rval = fscrb_write__internal(mi, st, fmt, msgfmt, false, ap);
    va_end (ap);
    return rval;
}
#define fscrb_write(st, fmt, msg, ...) (fscrb_write)(get_meta_info((st)->name), (st), (fmt), (msg), ##__VA_ARGS__)

// `fscribe_writeln`
// doc...
static inline
int (fscrb_writeln)(struct scrb_meta_info const mi, 
                    struct scrb_stream * const st,
                    struct scrb_format const * const fmt, 
                    char const * const msgfmt, ...)
{
    va_list ap;
    va_start (ap, msgfmt);
    int const rval = fscrb_write__internal(mi, st, fmt, msgfmt, true, ap);
    va_end (ap);
    return rval;
}
#define fscrb_writeln(st, fmt, msg, ...) (fscrb_writeln)(get_meta_info((st)->name), (st), (fmt), (msg), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

