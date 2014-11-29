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

struct streaminfo {
    FILE * filestream;
    char const * mode;
};

struct scrb_stream {
	char const * name;
    bool const synchronize;
    struct streaminfo const stream;
    spinlock_t rwlock;
};

typedef struct scrb_stream scrb_stream;

scrb_stream const stream_out_default;

scrb_stream const stream_in_default;

scrb_stream const stream_err_default;

#if SCRIBE_DEBUG
scrb_stream const scrb_dbg_default;
#endif

static inline
void scrb_init_defaults(scrb_stream const * const outstream,
                        scrb_stream const * const instream,
                        scrb_stream const * const errstream, ...)
{
    static volatile int initialized = 0;
#if defined(SCRIBE_WINDOWS)
    if (0 == InterlockedCompareExchange(&initialized, 1, 0)) {
#else    
    if (__sync_bool_compare_and_swap(&initialized, 0, 1)) {
#endif
        memcpy((void *)&stream_out_default, outstream, sizeof(scrb_stream));
        memcpy((void *)&stream_in_default, instream, sizeof(scrb_stream));
        memcpy((void *)&stream_err_default, errstream, sizeof(scrb_stream));
#if SCRIBE_DEBUG
        va_list ap;
        va_start (ap, errstream);
        scrb_stream const * const dbgstream = va_arg(ap, scrb_stream *);
        memcpy((void *)&scrb_dbg_default, dbgstream, sizeof(scrb_stream));
        va_end (ap); 
#endif
    }
}

static inline
char const * scrb_stream_name__internal(scrb_stream const * const st)
{
    return (NULL == st ? NULL : st->name);
}

extern
scrb_stream * scrb_open_stream__internal(char const * const path, char const * const mode, bool const synchronize);

extern
void scrb_close_stream__internal(scrb_stream ** streamptr);

extern
int scrb_swap_filepath(scrb_stream * const st, char const * const newfilepath);

extern
int scrb_swap_filedes(scrb_stream * const st, FILE * const newfd, char const * const name);

extern
void scrb_flush_stream__internal(scrb_stream * const st);

#ifdef __cplusplus
}
#endif

#endif

