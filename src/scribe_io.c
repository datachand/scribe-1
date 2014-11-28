//
// IO operations for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "scribe_conf.h"
#include "scribe_debug.h"
#include "scribe_format.h"
#include "scribe_stream.h"
#include "scribe_types.h"
#include "scribe_utils.h"
#include "spinlock.h"

#define SCRB_MSGBUFFCAPACITY 512

// define at compile time (or before ``#include "scribe.h"``) if necessary.
#ifndef SCRB_MAXMSGLEN
#define SCRB_MAXMSGLEN (16 * SCRB_MSGBUFFCAPACITY)
#endif

#define SCRB_BUFFCAPACITY 1024

static
char const * build_alloced(char const * const msgfmt, va_list ap)
{
    char * msg = NULL;
    uint64_t retlen;
    uint64_t trysize = SCRB_MSGBUFFCAPACITY;
    do {
        trysize *= 2;
        msg = realloc(msg, trysize * sizeof(char));
        if (NULL == msg) {
            break;
        }
        retlen = vsnprintf(msg, trysize, msgfmt, ap);
        if (retlen < trysize) {
            break;
        }
    } while(trysize <= (SCRB_MAXMSGLEN/2));
    return msg;
}

int scrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
			             struct scrb_format const * const fmt, char const * const msg, bool const newline)
{
    char printbuff[SCRB_BUFFCAPACITY];
    char const * const wrt = scrb_build_msg(mi, fmt, printbuff, SCRB_BUFFCAPACITY, msg, newline);
    
    if (unlikely(NULL == wrt)) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to build log message, message length was %llu", strlen(msg));
#endif
        goto error;
    }
    
    if (st->synchronize) {
        thread_lock_acquire((spinlock_t *)&st->rwlock);
    }

    int const rc = fputs(wrt, st->stream.filestream);
    
    if (st->synchronize) {
        thread_lock_release((spinlock_t *)&st->rwlock);
    }
   
    // the message builder had to allocate space,
    // so we'll have to free it. 
    if (unlikely(wrt != printbuff)) {
        free((void *) wrt);
    }

    return rc != EOF ? (SCRIBE_Success) : (SCRIBE_Failure);
error:
    return (SCRIBE_Failure);
}

int fscrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
                          struct scrb_format const * const fmt, char const * const msgfmt, 
                          bool const newline, va_list ap)
{
    bool usebuff = true;
    char const * msgalloc = NULL;
	char msgbuff[SCRB_MSGBUFFCAPACITY];
    uint64_t const retlen = vsnprintf(msgbuff, SCRB_MSGBUFFCAPACITY, msgfmt, ap);

	if (unlikely(retlen >= SCRB_MSGBUFFCAPACITY)) {
	    usebuff = false;
        msgalloc = build_alloced(msgfmt, ap);
        if (NULL == msgalloc) {
 #if SCRIBE_DEBUG
            scrb_debug_write("Failed to build log message.");
#endif
            errno = ENOMEM;
            goto error;
        } 
    }
 
    char printbuff[SCRB_BUFFCAPACITY];
    char const * wrt;
    
    if (likely(usebuff)) {
        wrt = scrb_build_msg(mi, fmt, printbuff, SCRB_BUFFCAPACITY, msgbuff, newline);
    } else {
        wrt = scrb_build_msg(mi, fmt, printbuff, SCRB_BUFFCAPACITY, msgalloc, newline);
        free((void *) msgalloc);
    }
    
    if (unlikely(NULL == wrt)) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to build write string, length was %llu", strlen(wrt));
#endif
        goto error;
    }

    if (st->synchronize) {
        thread_lock_acquire((spinlock_t *)&st->rwlock);
    }
    
    int const rc = fputs(wrt, st->stream.filestream);
    
    if (st->synchronize) {
        thread_lock_release((spinlock_t *)&st->rwlock);
    }
	
    // the message builder had to allocate space,
    // so we'll have to free it. 
    if (unlikely(wrt != printbuff)) {
        free((void *) wrt);
    }

    return rc != EOF ? (SCRIBE_Success) : (SCRIBE_Failure);
error:
	return (SCRIBE_Failure);
}

#undef SCRB_MSGBUFFCAPACITY
#undef SCRB_BUFFCAPACITY

