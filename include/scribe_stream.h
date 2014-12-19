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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "scribe_conf.h"
#include "scribe_loglevel.h"
#include "scribe_return_types.h"
#include "spinlock.h"

#if defined(SCRIBE_WINDOWS)
#include "Windows.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

struct streaminfo {
    char const * const mode;
    FILE * const filestream;
};

struct scrb_stream {
	char const * name;
    bool const synchronize;
    enum scrb_loglevel const severity;
    struct streaminfo stream;
    struct spinlock rwlock;
};

static inline
char const * scrb_stream_name_internal(struct scrb_stream const * const st)
{
    return st->name;
}

static inline
enum scrb_loglevel scrb_stream_severity_internal(struct scrb_stream const * const st)
{
    return st->severity;
}

extern
struct scrb_stream * scrb_open_stream_internal(char const * const path, 
                                                char const * const mode, 
                                                bool const synchronize,
                                                enum scrb_loglevel const severity);

extern
void scrb_close_stream_internal(struct scrb_stream ** streamptr);

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
void scrb_flush_stream_internal(struct scrb_stream * const st);

#ifdef __cplusplus
}
#endif

#endif

