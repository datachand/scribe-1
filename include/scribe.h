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
#include "scribe_format.h"
#include "scribe_io.h"
#include "scribe_loglevel.h"
#include "scribe_metainfo.h"
#include "scribe_return_types.h"
#include "scribe_stream.h"
#include "scribe_utils.h"
#include "spinlock.h"

#if defined(SCRIBE_WINDOWS)
#include "Windows.h"
#endif

#define LVL_TO_DEBUG  (LVL_DEBUG)
#define LVL_TO_TRACE  (LVL_DEBUG | LVL_TRACE)
#define LVL_TO_INFO   (LVL_DEBUG | LVL_TRACE | LVL_INFO)
#define LVL_TO_NOTICE (LVL_DEBUG | LVL_TRACE | LVL_INFO | LVL_NOTICE)
#define LVL_TO_WARN   (LVL_DEBUG | LVL_TRACE | LVL_INFO | LVL_NOTICE | LVL_WARN)
#define LVL_TO_ALERT  (LVL_DEBUG | LVL_TRACE | LVL_INFO | LVL_NOTICE | LVL_WARN | LVL_ALERT)
#define LVL_TO_ERROR  (LVL_DEBUG | LVL_TRACE | LVL_INFO | LVL_NOTICE | LVL_WARN | LVL_ALERT | LVL_ERROR)
#define LVL_TO_EMERG  (LVL_DEBUG | LVL_TRACE | LVL_INFO | LVL_NOTICE | LVL_WARN | LVL_ALERT | LVL_ERROR | LVL_EMERG)
#define LVL_ALL       (LVL_DEBUG | LVL_TRACE | LVL_INFO | LVL_NOTICE | LVL_WARN | LVL_ALERT | LVL_ERROR | LVL_EMERG)

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
            .severity    = LVL_ALL, 
            .stream = {
                .mode       = NULL,
                .filestream = stdout,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        struct scrb_stream instream = (struct scrb_stream) {
            .name        = "stdin",
            .synchronize = true,
            .severity    = LVL_ALL,
            .stream = {
                .mode       = NULL,
                .filestream = stdin,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        struct scrb_stream errstream = (struct scrb_stream) {
            .name        = "stderr",
            .synchronize = true,
            .severity    = LVL_ALL,
            .stream = {
                .mode       = NULL,
                .filestream = stderr,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY) 
        };

#if SCRIBE_DEBUG
        FILE * const dbg_filestream = fopen("scribedebug.log", "a");
        struct scrb_stream dbgstream = (struct scrb_stream) {
            .name        = "debug",
            .synchronize = true,
            .severity    = LVL_ALL,
            .stream = {
                .mode       = NULL,
                .filestream = dbg_filestream,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        if (NULL == dbgstream.stream.filestream) {
            return (SCRB_Failure);
        }

        scrb_init_defaults(&outstream, &instream, &errstream, &dbgstream);
        fprintf(dbgstream.stream.filestream, "----- scribe init :: version (%s) :: compile date (%s %s) :: compiled as "SCRB_LANGUAGE" by "SCRB_CC" ver. " SCRB_CC_VER " :: pid (%d) -----\n",
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

// `scrb_stream_severity`
// doc...
static inline
uint16_t scrb_stream_severity(struct scrb_stream const * const st)
{
    return scrb_stream_severity__internal(st);
}

// `scribe_open_file`
// doc...
static inline
struct scrb_stream * scrb_open_stream(char const * const path, 
                                      char const * const mode, 
                                      bool const synchronize,
                                      uint16_t const severity)
{
    return scrb_open_stream__internal(path, mode, synchronize, severity);
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
int (scrb_log)(struct scrb_meta_info const mi, 
               struct scrb_stream * const st, 
               struct scrb_format const * const fmt, 
               uint16_t const severity,
               char const * const msg)
{
    if (severity & st->severity) {
        return scrb_log__internal(mi, st, fmt, severity, msg, false); 
    } else {
        return SCRB_Success;
    }
}
#define scrb_log(st, fmt, sv, msg) (scrb_log)(get_meta_info((st)->name), (st), (fmt), (sv), (msg))

// `scribe_writeln`
// doc...
static inline
int (scrb_logln)(struct scrb_meta_info const mi, 
                 struct scrb_stream * const st, 
                 struct scrb_format const * const fmt, 
                 uint16_t const severity,
                 char const * const msg)
{
    if (severity & st->severity) {
        return scrb_log__internal(mi, st, fmt, severity, msg, true); 
    } else {
        return SCRB_Success;
    }
}
#define scrb_logln(st, fmt, sv, msg) (scrb_logln)(get_meta_info((st)->name), (st), (fmt), (sv), (msg))

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
int (scrb_flog)(struct scrb_meta_info const mi, 
                struct scrb_stream * const st,
                struct scrb_format const * const fmt, 
                uint16_t const severity,
                char const * const msgfmt, ...)
{
    if (severity & st->severity) {
        va_list ap;
        va_start (ap, msgfmt);
        int const rval = scrb_flog__internal(mi, st, fmt, severity, msgfmt, false, ap);
        va_end (ap);
        return rval;
    } else {
        return SCRB_Success;
    }
}
#define scrb_flog(st, fmt, sv, msg, ...) (scrb_flog)(get_meta_info((st)->name), (st), (fmt), (sv), (msg), ##__VA_ARGS__)

// `fscribe_writeln`
// doc...
static inline
int (scrb_flogln)(struct scrb_meta_info const mi, 
                  struct scrb_stream * const st,
                  struct scrb_format const * const fmt, 
                  uint16_t const severity,
                  char const * const msgfmt, ...)
{
    if (severity & st->severity) {
        va_list ap;
        va_start (ap, msgfmt);
        int const rval = scrb_flog__internal(mi, st, fmt, severity, msgfmt, true, ap);
        va_end (ap);
        return rval;
    } else {
        return SCRB_Success;
    }
}
#define scrb_flogln(st, fmt, sv, msg, ...) (scrb_flogln)(get_meta_info((st)->name), (st), (fmt), (sv), (msg), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

