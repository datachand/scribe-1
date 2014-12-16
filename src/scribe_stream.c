// 
// stream implementation for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "scribe.h"
#include "scribe_conf.h"
#include "scribe_debug.h"
#include "scribe_types.h"
#include "scribe_stream.h"
#include "scribe_utils.h"
#include "spinlock.h"

struct scrb_stream * scrb_open_stream__internal(char const * const path, 
                                                char const * const mode, 
                                                bool const synchronize)
{
	if (NULL == path) {
#if SCRIBE_DEBUG
		scrb_debug_write("NULL path string.");
#endif
		errno = EINVAL;
		goto error;
	} else if (NULL == mode) {
#if SCRIBE_DEBUG
        scrb_debug_write("NULL mode string.h");
#endif
        errno = EINVAL;
        goto error;
    }

    FILE * const filestream = fopen(path, mode);
	if (NULL == filestream) {
#if SCRIBE_DEBUG
		scrb_debug_write("Failed to open file %s: %s", path, strerror(errno));
#endif
		goto error;
	}
	
    struct scrb_stream tmp = {
		.name        = stringdup(path),
		.synchronize = synchronize,
        .stream = {
            .mode       = mode,
            .filestream = filestream,
        },
        .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
	};

	if (NULL == tmp.name) {
#if SCRIBE_DEBUG
		scrb_debug_write("Failed to copy name.");
#endif
		fclose(filestream);
		errno = ENOMEM;
		goto error;
	}

	struct scrb_stream * new_stream = malloc(sizeof(struct scrb_stream));
	if (NULL == new_stream) {
#if SCRIBE_DEBUG
		scrb_debug_write("Failed to create new stream object.");
#endif
		free((void *)tmp.name);
		fclose(filestream);
		errno = ENOMEM;
		goto error;
	}

	memcpy(new_stream, &tmp, sizeof(struct scrb_stream));
	return new_stream;
error:
	return NULL;
}

void scrb_close_stream__internal(struct scrb_stream ** streamptr)
{
	if (NULL != streamptr && NULL != *streamptr
     && *streamptr != scrb_stdout
     && *streamptr != scrb_stdin
     && *streamptr != scrb_stderr
#if SCRIBE_DEBUG
     && *streamptr != scrb_debug
#endif
     ) {
		free((void *)(*streamptr)->name);
		if (0 != (*streamptr)->stream.filestream) {
			fclose((*streamptr)->stream.filestream);
		}
		free((void *)*streamptr);
		*streamptr = NULL;
	}
}

// Currently doesn't build with gcc 49
/*int scrb_swap_filepath(struct scrb_stream * const st, 
                       char const * const newfilepath, 
                       char const * const mode)
{
    if (NULL == newfilepath || NULL == mode) {
#if SCRIBE_DEBUG
        scrb_debug_write("Bad filepath or mode string.");
#endif
        errno = EINVAL;
        goto error;
    }

    FILE * const newfilestream = fopen(newfilepath, st->stream.mode);
    if (NULL == newfilestream) {
#if SCRIBE_DEBUG
        scrb_debug_write("Failed to open new file %s in mode \"%s\"", newfilepath, st->stream.mode);
#endif
        goto error;
    }
    
    if (st->synchronize) {
        thread_lock_acquire(&st->rwlock);
    }
    int const newfiledes = fileno(newfilestream);
    st->name = newfilepath;
    fclose(st->stream.filestream);
    *(struct streaminfo *)&st->stream = (struct streaminfo) { 
                                            .filestream    = newfilestream,
                                            .filedes       = newfiledes, 
                                            .mode          = mode,
                                        };
    if (st->synchronize) {
        thread_lock_release(&st->rwlock);
    }

    return (SCRB_Success);
error:
    return (SCRB_Failure);
}

int scrb_swap_filestream(struct scrb_stream * const st, 
                         FILE * const newfilestream, 
                         char const * const mode, 
                         char const * const name)
{
    if (NULL == newfilestream) {
#if SCRIBE_DEBUG
        scrb_debug_write("Bad file stream.");
#endif
        errno = EINVAL;
        goto error;
    } else if(NULL == name || NULL == mode) {
#if SCRIBE_DEBUG
        scrb_debug_write("Bad name or mode string.");
#endif
        errno = EINVAL;
        goto error;
    }
    
    if (st->synchronize) {
        thread_lock_acquire(&st->rwlock);
    }
    int const newfiledes = fileno(newfilestream);
    st->name = name;
    fclose(st->stream.filestream);
    *(struct streaminfo *)&st->stream = (struct streaminfo) { 
                                            .filestream    = newfilestream,
                                            .filedes       = newfiledes, 
                                            .mode          = mode,
                                        };
    if (st->synchronize) {
        thread_lock_release(&st->rwlock);
    }

    return (SCRB_Success);
error:
    return (SCRB_Failure);
}

extern
int scrb_swap_filedes(struct scrb_stream * const st,
                      int const newfiledes, 
                      char const * const mode, 
                      char const * const name)
{
    if (0 > newfiledes) {
#if SCRIBE_DEBUG
        scrb_debug_write("Bad file descriptor");
#endif
        errno = EINVAL;
        goto error;
    } else if (NULL == name || NULL == mode) {
#if SCRIBE_DEBUG
        scrb_debug_write("Bad name or mode string.");
#endif
        errno = EINVAL;
        goto error;
    }

    if (st->synchronize) {
        thread_lock_acquire(&st->rwlock);
    }
    FILE * const newfilestream = fdopen(newfiledes, mode);
    st->name = name;
    fclose(st->stream.filestream);
    *(struct streaminfo *)&st->stream = (struct streaminfo) { 
                                            .filestream    = newfilestream,
                                            .filedes       = newfiledes, 
                                            .mode          = mode,
                                        };
    if (st->synchronize) {
        thread_lock_release(&st->rwlock);
    }

    return (SCRB_Success);
error:
    return (SCRB_Failure);
}
*/
void scrb_flush_stream__internal(struct scrb_stream * const st)
{
    if (NULL != st) {
        fflush(st->stream.filestream);
    }
}

