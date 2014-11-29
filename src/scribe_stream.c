// 
// stream implementation for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scribe_conf.h"
#include "scribe_debug.h"
#include "scribe_types.h"
#include "scribe_stream.h"
#include "scribe_utils.h"
#include "spinlock.h"

scrb_stream * scrb_open_stream__internal(char const * const path, char const * const mode, bool const synchronize)
{
	if (NULL == path || NULL == mode) {
#if SCRIBE_DEBUG
		scrb_debug_write("NULL path and/or mode string.");
#endif
		errno = EINVAL;
		goto error;
	}

	FILE * const fd = fopen(path, mode);
	if (NULL == fd) {
#if SCRIBE_DEBUG
		scrb_debug_write("Failed to open file %s in mode %s", path, mode);
#endif
		goto error;
	}

	struct scrb_stream tmp = {
		.name = stringdup(path),
		.synchronize = synchronize,
        .stream = {
            .filestream = fd,
            .mode = mode
        },
        .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
	};

	if (NULL == tmp.name) {
#if SCRIBE_DEBUG
		scrb_debug_write("Failed to copy name.");
#endif
		fclose(fd);
		errno = ENOMEM;
		goto error;
	}

	scrb_stream * new_stream = malloc(sizeof(struct scrb_stream));
	if (NULL == new_stream) {
#if SCRIBE_DEBUG
		scrb_debug_write("Failed to create new stream object.");
#endif
		free((void *)tmp.name);
		fclose(fd);
		errno = ENOMEM;
		goto error;
	}

	memcpy(new_stream, &tmp, sizeof(struct scrb_stream));
	return new_stream;
error:
	return NULL;
}

void scrb_close_stream__internal(scrb_stream ** streamptr)
{
	if (NULL != streamptr && NULL != *streamptr) {
		free((void *)(*streamptr)->name);
		if (0 != (*streamptr)->stream.filestream) {
			fclose((*streamptr)->stream.filestream);
		}
		free((void *)*streamptr);
		*streamptr = NULL;
	}
}

int scrb_swap_filepath(scrb_stream * const st, char const * const newfilepath)
{
    FILE * const newfd = fopen(newfilepath, st->stream.mode);
    if (NULL == newfd) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to open new file %s in mode \"%s\"", newfilepath, st->stream.mode);
#endif
        goto error;
    } else {
        if (st->synchronize) {
            thread_lock_acquire(&st->rwlock);
        }
        st->name = newfilepath;
        fclose(st->stream.filestream);
        *(struct streaminfo *)&st->stream = (struct streaminfo) { .filestream = newfd, .mode = st->stream.mode };
        if (st->synchronize) {
            thread_lock_release(&st->rwlock);
        }
    }
    return (SCRIBE_Success);
error:
    return (SCRIBE_Failure);
}

int scrb_swap_filedes(scrb_stream * const st, FILE * const newfd, char const * const name)
{
    if (NULL == newfd) {
#if SCRIBE_DEBUG
        scrb_debug_write("Bad file descriptor.");
#endif
        goto error;
    } else {
        if (st->synchronize) {
            thread_lock_acquire(&st->rwlock);
        }
        st->name = name;
        fclose(st->stream.filestream);
        *(struct streaminfo *)&st->stream = (struct streaminfo) { .filestream = newfd, .mode = st->stream.mode };
        if (st->synchronize) {
            thread_lock_release(&st->rwlock);
        }
    }
    return (SCRIBE_Success);
error:
    return (SCRIBE_Failure);
}

void scrb_flush_stream__internal(scrb_stream * const st)
{
    if (NULL != st) {
        fflush(st->stream.filestream);
    }
}

