//
// IO operations for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "scribe_conf.h"
#include "scribe_format.h"
#include "scribe_stream.h"
#include "scribe_types.h"
#include "scribe_utils.h"
#include "spinlock.h"

int scrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
			             struct scrb_format const * const fmt, char const * const msg)
{
	char const * const wrt = scrb_build_msg(mi, fmt, msg);
    if (NULL == wrt) {
#ifdef SCRIBE_DEBUG
        fprintf(stderr, "Failed to build log message.");
#endif
        goto error;
    } else {
        if (st->synchronize) {
            thread_lock_acquire(st->rwlock);
        }
        fputs(st->filestream, wrt);
        free(wrt);
        if (st->synchronize) {
            thread_lock_release(st->rwlock);
        }
    }
	return (SCRIBE_Success);
error:
    return (SCRIBE_Failure);
}

int fscrb_write__internal(struct scrb_meta_info const mi, scrb_stream const * const st,
                          struct scrb_format const * const fmt, char const * const msgfmt, va_list ap)
{
	char * msg;
	vasprintf(&msg, msgfmt, ap);

	if (NULL == msg) {
#if SCRIBE_DEBUG
		fprintf(stderr, "Failed to create log message.");
#endif
		errno = ENOMEM;
		goto error;
	}

	char const * const wrt = scrb_build_msg(mi, fmt, msg);
    if (NULL == wrt) {
#ifdef SCRIBE_DEBUG
        fprintf(stderr, "Failed to build log message.");
#endif
    } else {
        if (st->synchronize) {
            thread_lock_acquire(st->rwlock);
        }
        fputs(st->filestream, wrt);
        free(wrt);
        if (st->synchronize) {
            thread_lock_release(st->rwlock);
        }
    }
	return (SCRIBE_Success);
error:
	return (SCRIBE_Failure);
}

