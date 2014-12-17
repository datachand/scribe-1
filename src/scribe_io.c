//
// IO operations for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <sys/mman.h>

#include "scribe_conf.h"
#include "scribe_debug.h"
#include "scribe_format.h"
#include "scribe_stream.h"
#include "scribe_return_types.h"
#include "scribe_utils.h"
#include "spinlock.h"

#define SCRB_MSGBUFFCAPACITY 512

// define at compile time (or before ``#include "scribe.h"``) if necessary.
#ifndef SCRB_MAXMSGLEN
#define SCRB_MAXMSGLEN (4 * SCRB_MSGBUFFCAPACITY)
#endif

#define SCRB_BUFFCAPACITY 1024

static
char const * build_alloced(char const * const msgfmt, va_list ap)
{
    char * msg       = NULL;
    uint64_t trysize = SCRB_MSGBUFFCAPACITY;
    uint64_t retlen;
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

int scrb_putstr__internal(struct scrb_stream * const st, char const * const msg, bool const newline)
{
    int ret = 0;
    if(likely(msg != NULL)) {
        if (st->synchronize) {
            thread_lock_acquire(&st->rwlock);
        }        
        ret = fputs(msg, st->stream.filestream);
        if (newline) {
            fputc('\n', st->stream.filestream);
        }
        if (st->synchronize) {
            thread_lock_release(&st->rwlock);
        }
    } else {
#if SCRIBE_DEBUG
        scrb_debug_write("NULL message string.");
#endif
        errno = EINVAL;
        goto error;
    }
    return (ret != EOF ? SCRB_Success : SCRB_Failure);
error:
    return (SCRB_Failure);
}

int scrb_log__internal(struct scrb_meta_info const mi, 
                       struct scrb_stream * const st,
                       struct scrb_format const * const fmt, 
                       uint16_t const severity,
                       char const * const msg, 
                       bool const newline)
{
    char printbuff[SCRB_BUFFCAPACITY];
    uint64_t msg_length    = 0;
    char const * const wrt = scrb_build_msg(severity, mi, fmt, printbuff, SCRB_BUFFCAPACITY, msg, &msg_length, newline);
    
    if (unlikely(NULL == wrt)) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to build log message");
#endif
        goto error;
    }
    
    if (st->synchronize) {
        thread_lock_acquire(&st->rwlock);
    }

    int const rc = fputs(wrt, st->stream.filestream);
    
    if (st->synchronize) {
        thread_lock_release(&st->rwlock);
    }
   
    // the message builder had to allocate space,
    // so we'll have to free it. 
    if (unlikely(wrt != printbuff)) {
        free((void *) wrt);
    }

    return rc != EOF ? (SCRB_Success) : (SCRB_Failure);
error:
    return (SCRB_Failure);
}

int scrb_flog__internal(struct scrb_meta_info const mi, 
                        struct scrb_stream * const st,
                        struct scrb_format const * const fmt, 
                        uint16_t const severity,
                        char const * const msgfmt, 
                        bool const newline, 
                        va_list ap)
{
    bool usebuff          = true;
    char const * msgalloc = NULL;
	char msgbuff[SCRB_MSGBUFFCAPACITY];
    uint64_t const retlen = vsnprintf(msgbuff, SCRB_MSGBUFFCAPACITY, msgfmt, ap);

	if (unlikely(retlen >= SCRB_MSGBUFFCAPACITY)) {
	    usebuff  = false;
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
    uint64_t msg_length = 0;
    
    if (likely(usebuff)) {
        wrt = scrb_build_msg(severity, mi, fmt, printbuff, SCRB_BUFFCAPACITY, msgbuff, &msg_length, newline);
    } else {
        wrt = scrb_build_msg(severity, mi, fmt, printbuff, SCRB_BUFFCAPACITY, msgalloc, &msg_length, newline);
        free((void *) msgalloc);
    }
    
    if (unlikely(NULL == wrt)) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to build write string.");
#endif
        errno = ENOMEM;
        goto error;
    }

    if (st->synchronize) {
        thread_lock_acquire(&st->rwlock);
    }
    
    int const rc = fputs(wrt, st->stream.filestream);
 
    if (st->synchronize) {
        thread_lock_release(&st->rwlock);
    }
	
    // the message builder had to allocate space,
    // so we'll have to free it. 
    if (unlikely(wrt != printbuff)) {
        free((void *) wrt);
    }

    return rc != EOF ? (SCRB_Success) : (SCRB_Failure);
error:
	return (SCRB_Failure);
}

#undef SCRB_MSGBUFFCAPACITY
#undef SCRB_BUFFCAPACITY

