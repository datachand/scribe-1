//
// Scribe initialization method
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "scribe.h"
#include "scribe_conf.h"
#include "scribe_debug.h"
#include "scribe_return_types.h"
#include "scribe_stream.h"

int scrb_init_internal(struct scrb_stream ** scrb_stdoutptr,
                       struct scrb_stream ** scrb_stdinptr,
                       struct scrb_stream ** scrb_stderrptr
#if SCRIBE_DEBUG
                       ,struct scrb_stream ** scrb_debugptr
#endif
                      )
{
#if defined(SCRIBE_WINDOWS)
    static volatile LONG initialized = 0;
    if (0 == InterlockedCompareExchange(&initialized, 1, 0)) {
#else    
    static volatile int initialized = 0;
    if (__sync_bool_compare_and_swap(&initialized, 0, 1)) {
#endif
        struct scrb_stream outstream = {
            .name        = "stdout",
            .synchronize = true,
            .severity    = LVL_ALL, 
            .stream = {
                .mode       = "a",
                .filestream = stdout,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        struct scrb_stream instream = {
            .name        = "stdin",
            .synchronize = true,
            .severity    = LVL_ALL,
            .stream = {
                .mode       = "a",
                .filestream = stdin,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        struct scrb_stream errstream = {
            .name        = "stderr",
            .synchronize = true,
            .severity    = LVL_ALL,
            .stream = {
                .mode       = "a",
                .filestream = stderr,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY) 
        };

        // allocate and initialized standard streams
#if defined(__cplusplus)
        *scrb_stdoutptr = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        *scrb_stdoutptr = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == *scrb_stdoutptr) {
            errno = ENOMEM;
            goto error;
        }

#if defined(__cplusplus)
        *scrb_stdinptr = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        *scrb_stdinptr = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == *scrb_stdinptr) {
            free(*scrb_stdoutptr);
            errno = ENOMEM;
            goto error;
        }

#if defined(__cplusplus)
        *scrb_stderrptr = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        *scrb_stderrptr = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == *scrb_stderrptr) {
            free(*scrb_stdoutptr);
            free(*scrb_stdinptr);
            errno = ENOMEM;
            goto error;
        }

#if SCRIBE_DEBUG
        FILE * const dbg_filestream = fopen("scribedebug.log", "a");
        struct scrb_stream dbgstream = {
            .name        = "debug",
            .synchronize = true,
            .severity    = LVL_ALL,
            .stream = {
                .mode       = NULL,
                .filestream = dbg_filestream,
            },
            .rwlock = spinlock_init(SCRIBE_RWLOCK_DELAY)
        };

        if (NULL == dbgstream.stream.filestream) {
            free(scrb_stdout);
            free(scrb_stdin);
            free(scrb_stderr);
            goto error;
        }

#if defined(__cplusplus)
        *scrb_debugptr = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        *scrb_debugptr = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == *scrb_debugptr) {
            free(*scrb_stdoutptr);
            free(*scrb_stdinptr);
            free(*scrb_stderrptr);
            errno = ENOMEM;
            goto error;
        }
        memcpy(*scrb_debugptr, &dbgstream, sizeof(struct scrb_stream));
#endif
        memcpy(*scrb_stdoutptr, &outstream, sizeof(struct scrb_stream));
        memcpy(*scrb_stdinptr, &instream, sizeof(struct scrb_stream));
        memcpy(*scrb_stderrptr, &errstream, sizeof(struct scrb_stream));
#if SCRIBE_DEBUG 
        fprintf(dbgstream.stream.filestream, 
                "----- successful scribe init :: version (%s) :: compile date (%s %s) "
                ":: compiled as "SCRB_LANGUAGE" by "SCRB_CC" ver. " SCRB_CC_VER " "
                ":: pid (%d) -----\n", SCRIBE_VERSION, __DATE__, __TIME__, (int) scrb_getpid());
#endif
    }
    return (SCRB_Success);
error:
    return (SCRB_Failure);
}
