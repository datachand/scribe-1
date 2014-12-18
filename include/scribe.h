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
#include <string.h>

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

#ifdef __cplusplus
extern "C" {
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
struct scrb_stream * scrb_stdout;

static
struct scrb_stream * scrb_stdin;

static
struct scrb_stream * scrb_stderr;

extern
int scrb_init_internal(struct scrb_stream ** scrb_stdoutptr,
                       struct scrb_stream ** scrb_stdinptr,
                       struct scrb_stream ** scrb_stderrptr
#if SCRIBE_DEBUG
                       ,struct scrb_stream ** scrb_debugptr
#endif
                      );

// `scrb_init`
// doc...
static inline
int scrb_init(void)
{
#if SCRIBE_DEBUG
    return scrb_init_internal(&scrb_stdout, &scrb_stdin, &scrb_stderr, &scrb_debug);
#else
    return scrb_init_internal(&scrb_stdout, &scrb_stdin, &scrb_stderr);
#endif
}

// `scrb_create_format`
// doc...
static inline
struct scrb_format * scrb_create_format(char const * const fmtstr, 
                                        void (*timehook)(char ** buff, 
                                                         size_t * len, 
                                                         SCRIBE_TIME_T ts))
{
    return scrb_create_format_internal(fmtstr, timehook);
}

//
//
static inline
void scrb_format_release(struct scrb_format ** fmtptr)
{
    scrb_format_release_internal(fmtptr);
}

// `scrb_stream_name`
// doc...
static inline
char const * scrb_stream_name(struct scrb_stream const * const st)
{
    return scrb_stream_name_internal(st);
}

// `scrb_stream_severity`
// doc...
static inline
uint16_t scrb_stream_severity(struct scrb_stream const * const st)
{
    return scrb_stream_severity_internal(st);
}

// `scribe_open_file`
// doc...
static inline
struct scrb_stream * scrb_open_stream(char const * const path, 
                                      char const * const mode, 
                                      bool const synchronize,
                                      uint16_t const severity)
{
    return scrb_open_stream_internal(path, mode, synchronize, severity);
}

// `scribe_close`
// doc...
static inline
void scrb_close_stream(struct scrb_stream ** streamptr)
{
    scrb_close_stream_internal(streamptr);
}

// `scribe_flush_stream`
// doc...
static inline
void scrb_flush_stream(struct scrb_stream * const st)
{
    scrb_flush_stream_internal(st);
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
    int rval = SCRB_Success;
    if (severity >= st->severity) {
        rval = scrb_log_internal(mi, st, fmt, severity, msg, false); 
    }
    return rval;
}
#define scrb_log(st, fmt, sv, msg) (scrb_log)(get_meta_info(), (st), (fmt), (sv), (msg))

// `scribe_writeln`
// doc...
static inline
int (scrb_logln)(struct scrb_meta_info const mi, 
                 struct scrb_stream * const st, 
                 struct scrb_format const * const fmt, 
                 uint16_t const severity,
                 char const * const msg)
{
    int rval = SCRB_Success;
    if (severity >= st->severity) {
        rval = scrb_log_internal(mi, st, fmt, severity, msg, true); 
    }
    return rval;
}
#define scrb_logln(st, fmt, sv, msg) (scrb_logln)(get_meta_info(), (st), (fmt), (sv), (msg))

// `scribe_writestr`
// doc...
static inline
int scrb_putstr(struct scrb_stream * const st, char const * const msg)
{
    return scrb_putstr_internal(st, msg, false);
}

// `scribe_writestrln`
// doc...
static inline
int scrb_putstrln(struct scrb_stream * const st, char const * const msg)
{
    return scrb_putstr_internal(st, msg, true);
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
    int rval = SCRB_Success;
    if (severity >= st->severity) {
        va_list ap;
        va_start (ap, msgfmt);
        rval = scrb_flog_internal(mi, st, fmt, severity, msgfmt, false, ap);
        va_end (ap);
    }
    return rval;
}
#define scrb_flog(st, fmt, sv, msg, ...) (scrb_flog)(get_meta_info(), (st), (fmt), (sv), (msg), ##__VA_ARGS__)

// `fscribe_writeln`
// doc...
static inline
int (scrb_flogln)(struct scrb_meta_info const mi, 
                  struct scrb_stream * const st,
                  struct scrb_format const * const fmt, 
                  uint16_t const severity,
                  char const * const msgfmt, ...)
{
    int rval = SCRB_Success;
    if (severity >= st->severity) {
        va_list ap;
        va_start (ap, msgfmt);
        rval = scrb_flog_internal(mi, st, fmt, severity, msgfmt, true, ap);
        va_end (ap);
    }
    return rval;
}
#define scrb_flogln(st, fmt, sv, msg, ...) (scrb_flogln)(get_meta_info(), (st), (fmt), (sv), (msg), ##__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif

