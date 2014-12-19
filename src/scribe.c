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

struct scrb_stream * scrb_stdout = NULL;
struct scrb_stream * scrb_stdin  = NULL;
struct scrb_stream * scrb_stderr = NULL;
#if SCRIBE_DEBUG
struct scrb_stream * scrb_debug  = NULL;
#endif

int scrb_init(void)
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
        scrb_stdout = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        scrb_stdout = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == scrb_stdout) {
            errno = ENOMEM;
            goto error;
        }
#if defined(__cplusplus)
        scrb_stdin = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        scrb_stdin = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == scrb_stdin) {
            free(scrb_stdout);
            errno = ENOMEM;
            goto error;
        }

#if defined(__cplusplus)
        scrb_stderr = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        scrb_stderr = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == scrb_stderr) {
            free(scrb_stdout);
            free(scrb_stdin);
            errno = ENOMEM;
            goto error;
        }

#if SCRIBE_DEBUG
        FILE * const dbg_filestream = fopen("scribedebug.log", "a");
        struct scrb_stream dbgstream = {
            .name        = "scribedebug.log",
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
        scrb_debug = (struct scrb_stream *) malloc(sizeof(struct scrb_stream));
#else
        scrb_debug = malloc(sizeof(struct scrb_stream));
#endif
        if (NULL == scrb_debug) {
            free(scrb_stdout);
            free(scrb_stdin);
            free(scrb_stderr);
            errno = ENOMEM;
            goto error;
        }
        memcpy(scrb_debug, &dbgstream, sizeof(struct scrb_stream));
#endif
        memcpy(scrb_stdout, &outstream, sizeof(struct scrb_stream));
        memcpy(scrb_stdin, &instream, sizeof(struct scrb_stream));
        memcpy(scrb_stderr, &errstream, sizeof(struct scrb_stream));
#if SCRIBE_DEBUG 
        fprintf(dbgstream.stream.filestream, 
                "----- successful scribe init :: compile date (%s %s) "
                ":: compiled as " SCRB_LANGUAGE " by " SCRB_CC " ver. " SCRB_CC_VER " "
                ":: pid (%d) -----\n", __DATE__, __TIME__, (int) scrb_getpid());
#endif
    }
    return (SCRB_Success);
error:
    return (SCRB_Failure);
}

