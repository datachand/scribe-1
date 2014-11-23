// 
// stream implementation for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_STREAM_H
#define SCRIBE_STREAM_H

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scribe_conf.h"
#include "scribe_types.h"
#include "spinlock.h"

struct scrb_stream {
	char const * const name;
    bool const readable;
	bool const writeable;
	bool const synchronize;
    bool const memmap;
    struct {
        FILE * const filestream;
        char * const memmap;        // unused if memmap is false.
        uint64_t offset;            // " ... "
        uint64_t const pagesize;    // " ... "
    } stream;
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
    static bool initialized = false;
    if (!initialized) {
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
        initialized = true;
    } 
}

static inline
char const * scrb_stream_name__internal(scrb_stream const * const st)
{
    if (NULL == st) {
        goto error;
    }
    return st->name;
error:
    return NULL;
}

extern
scrb_stream * scrb_open_stream__internal(char const * const path, char const * const mode,
                                         bool const synchronize, bool const memmap);

extern
void scrb_close_stream__internal(scrb_stream ** streamptr);

extern
void scrb_flush_stream__internal(scrb_stream * const st);

extern
void scrb_purge_stream__internal(scrb_stream * const st);

#endif

