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
#include <sys/mman.h>
#include <unistd.h>

#include "scribe_conf.h"
#include "scribe_debug.h"
#include "scribe_types.h"
#include "scribe_stream.h"
#include "scribe_utils.h"
#include "spinlock.h"

scrb_stream * scrb_open_stream__internal(char const * const path, char const * const mode,
                                         bool const synchronize, bool const memmap)
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
		scrb_debug_write("Failed to open file.");
#endif
		goto error;
	}

	bool const readable = (NULL == charindex(mode, 'r') && NULL == charindex(mode, '+')) ? false : true;
	bool const writeable = (NULL == charindex(mode, 'w') && NULL == charindex(mode, '+')) ? false : true;

	struct scrb_stream tmp = {
		.name = stringdup(path),
		.readable = readable,
		.writeable = writeable,
		.synchronize = synchronize,
        .memmap = memmap,
        .stream = {
            .filestream = fd,
            .memmap = NULL,
            .offset = 0,
            .pagesize = false == memmap ? 0 : sysconf(_SC_PAGESIZE)
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

void scrb_flush_stream__internal(scrb_stream * const st)
{
    if (NULL != st) {
        fflush(st->stream.filestream);
    }
}

