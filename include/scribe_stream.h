// 
// stream implementation for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_STREAM_H
#define SCRIBE_STREAM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "scribe_conf.h"
#include "scribe_types.h"
#include "spinlock.h"

#if defined(SCRIBE_WINDOWS)
#include "Windows.h"
#endif

struct streaminfo {
    char const * const mode;
    FILE * const filestream;
    int const filedes;
};

struct scrb_stream {
	char const * name;
    bool const synchronize;
    struct streaminfo stream;
    struct spinlock rwlock;
};

struct scrb_stream stream_out_default;

struct scrb_stream stream_in_default;

struct scrb_stream stream_err_default;

#if SCRIBE_DEBUG
struct scrb_stream scrb_dbg_default;
#endif

static inline
void scrb_init_defaults(struct scrb_stream * const outstream,
                        struct scrb_stream * const instream,
                        struct scrb_stream * const errstream, ...)
{
#if defined(SCRIBE_WINDOWS)
    static volatile LONG initialized = 0;
    if (0 == InterlockedCompareExchange(&initialized, 1, 0)) {
#else    
    static volatile int initialized = 0;
    if (__sync_bool_compare_and_swap(&initialized, 0, 1)) {
#endif
        memcpy(&stream_out_default, outstream, sizeof(struct scrb_stream));
        memcpy(&stream_in_default, instream, sizeof(struct scrb_stream));
        memcpy(&stream_err_default, errstream, sizeof(struct scrb_stream));
#if SCRIBE_DEBUG
        va_list ap;
        va_start (ap, errstream);
        struct scrb_stream * const dbgstream = va_arg(ap, struct scrb_stream *);
        memcpy(&scrb_dbg_default, dbgstream, sizeof(struct scrb_stream));
        va_end (ap); 
#endif
    }
}

static inline
char const * scrb_stream_name__internal(struct scrb_stream const * const st)
{
    return (NULL == st ? NULL : st->name);
}

extern
struct scrb_stream * scrb_open_stream__internal(char const * const path, 
                                                char const * const mode, 
                                                bool const synchronize);

extern
void scrb_close_stream__internal(struct scrb_stream ** streamptr);

// currently doesn't build with gcc, need to find a better way
// to swap info on the stream instream of casting away const (because that's
// as bad as it gets)
/*
extern
int scrb_swap_filepath(struct scrb_stream * const st, 
                       char const * const newfilepath, 
                       char const * const mode);

extern
int scrb_swap_filestream(struct scrb_stream * const st, 
                         FILE * const newfilestream, 
                         char const * const mode, 
                         char const * const name);

extern
int scrb_swap_filedes(struct scrb_stream * const st, 
                      int const newfiledes, 
                      char const * const mode, 
                      char const * const name);
*/
extern
void scrb_flush_stream__internal(struct scrb_stream * const st);

#ifdef __cplusplus
}
#endif

#endif

