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
#include "scribe_types.h"
#include "scribe_stream.h"
#include "spinlock.h"

scrb_stream * scrb_open_stream__internal(char const * const path, char const * const mode,
                                         bool const synchronize)
{
	if (NULL == path || NULL == mode) {
#if SCRIBE_DEBUG
		fprintf(stderr, "NULL path and/or mode string.\n");
#endif
		errno = EINVAL;
		goto error;
	}

	FILE * const fd = fopen(path, mode);
	if (NULL == fd) {
#if SCRIBE_DEBUG
		fprintf(stderr, "Failed to open file.\n");
#endif
		goto error;
	}

	bool const readable = (NULL == index(mode, 'r') && NULL == index(mode, '+')) ? false : true;
	bool const writeable = (NULL == index(mode, 'w') && NULL == index(mode, '+')) ? false : true;

	struct scrb_stream tmp = {
		.name = strdup(path),
		.filestream = fd,
		.readable = readable,
		.writeable = writeable,
		.synchronize = synchronize,
        .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
	};

	if (NULL == tmp.name) {
#if SCRIBE_DEBUG
		fprintf(stderr, "Failed to copy name.\n");
#endif
		fclose(fd);
		errno = ENOMEM;
		goto error;
	}

	scrb_stream * new_stream = malloc(sizeof(struct scrb_stream));
	if (NULL == new_stream) {
#if SCRIBE_DEBUG
		fprintf(stderr, "Failed to create new stream object.\n");
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
		if (0 != (*streamptr)->filestream) {
			fclose((*streamptr)->filestream);
		}
		free((void *)*streamptr);
		*streamptr = NULL;
	}
}

int scrb_flush_stream__internal(scrb_stream * const st)
{
    (void) st;
    return (SCRIBE_Success);
}

int scrb_purge_stream__internal(scrb_stream * const st)
{
    (void) st;
    return (SCRIBE_Success);
}

