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

#define MSGBUFFCAPACITY 512
#define BUFFCAPACITY 1024

/*static
int mmap_write(char const * const msg, uint64_t const msglen, struct mmstream * const mmst)
{
    return (SCRIBE_Success);
}

static
int mmap_remap(struct mmstream * const mmst)
{
    return (SCRIBE_Success);
}
*/
int scrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
			             struct scrb_format const * const fmt, char const * const msg, bool const newline)
{
    char printbuff[BUFFCAPACITY];
    char const * const wrt = scrb_build_msg(mi, fmt, printbuff, BUFFCAPACITY, msg, newline);
    
    if (unlikely(NULL == wrt)) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to build log message, message length was %llu", strlen(msg));
#endif
        goto error;
    } else {
        if (st->synchronize) {
            thread_lock_acquire((spinlock_t *)&st->rwlock);
        }
        fputs(wrt, st->stream.filestream);
        if (st->synchronize) {
            thread_lock_release((spinlock_t *)&st->rwlock);
        }
    }
   
    // the message builder had to allocate space,
    // so we'll have to free it. 
    if (wrt != printbuff) {
        free((void *) wrt);
    }

    return (SCRIBE_Success);
error:
    return (SCRIBE_Failure);
}

int fscrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
                          struct scrb_format const * const fmt, char const * const msgfmt, 
                          bool const newline, va_list ap)
{
	char msgbuff[MSGBUFFCAPACITY];
	//vasprintf(&msg, msgfmt, ap);
    uint64_t const retlen = vsnprintf(msgbuff, MSGBUFFCAPACITY, msgfmt, ap);

    // TODO: Find a better solution than failing when vsnprintf() runs out of room.
	if (unlikely(retlen >= MSGBUFFCAPACITY)) {
#if SCRIBE_DEBUG
	    scrb_debug_write("Failed to build log message, message format length was %llu", strlen(msgfmt));
#endif
		errno = ENOMEM;
		goto error;
	} else {
        
    }
    
    char printbuff[BUFFCAPACITY];
    char const * const wrt = scrb_build_msg(mi, fmt, printbuff, BUFFCAPACITY, msgbuff, newline);
    
    if (unlikely(NULL == wrt)) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to build write string, length was %llu", strlen(wrt));
#endif
    } else {
        if (st->synchronize) {
            thread_lock_acquire((spinlock_t *)&st->rwlock);
        }
        fputs(wrt, st->stream.filestream);
        if (st->synchronize) {
            thread_lock_release((spinlock_t *)&st->rwlock);
        }
    }
	
    // the message builder had to allocate space,
    // so we'll have to free it. 
    if (wrt != printbuff) {
        free((void *) wrt);
    }

    return (SCRIBE_Success);
error:
	return (SCRIBE_Failure);
}

#undef MSGBUFFCAPACITY
#undef BUFFCAPACITY

