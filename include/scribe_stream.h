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
#include <string.h>

#include "scribe_conf.h"
#include "scribe_stream.h"
#include "scribe_types.h"
#include "spinlock.h"

struct scrb_stream {
	char const * const name;
	FILE * const filestream;
	bool const readable;
	bool const writeable;
	bool const synchronize;
	spinlock_t rwlock;
};

typedef struct scrb_stream scrb_stream;

scrb_stream const stream_out_default;

scrb_stream const stream_in_default;

scrb_stream const stream_err_default;

static inline
void scrb_init_defaults(scrb_stream const * const outstream,
                        scrb_stream const * const instream,
                        scrb_stream const * const errstream)
{
    static bool initialized = false;
    if (!initialized) {
        memcpy((void *)&stream_out_default, outstream, sizeof(scrb_stream));
        memcpy((void *)&stream_in_default, instream, sizeof(scrb_stream));
        memcpy((void *)&stream_err_default, errstream, sizeof(scrb_stream));
        initialized = true;
    }
}

static inline
char const * scrb_stream_name__internal(scrb_stream const * const st)
{
    if (NULL == st) {
#if SCRIBE_DEBUG
        fprintf(stderr, "NULL stream pointer.");
#endif
        goto error;
    }
    return st->name;
error:
    return NULL;
}

extern
scrb_stream * scrb_open_stream__internal(char const * const path, char const * const mode, bool const synchronize);

extern
void scrb_close_stream__internal(scrb_stream ** streamptr);

extern
int scrb_flush_stream__internal(scrb_stream * const st);

extern
int scrb_purge_stream__internal(scrb_stream * const st);

#endif

