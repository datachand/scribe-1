//
// Synchronized logging example for scribe usage.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <stdint.h>
#include <stdio.h>
#include "scribe.h"

#if defined(SCRIBE_OSX) || defined(SCRIBE_UNIX) || defined(SCRIBE_LINUX)
#include <pthread.h>
#define PTHREAD
#endif

#define NTHREADS 4
#define NREPEATS 1000000

struct thread_arg {
    uint8_t const thread_id;
    scrb_stream const * const logstream;
    scrb_format const * const logfmt;
};

static
void * threadlog(void * arg)
{
    struct thread_arg const thrdargs = *(struct thread_arg *) arg;
    
    uint8_t const id = thrdargs.thread_id;
    scrb_stream const * const st = thrdargs.logstream;
    scrb_format const * const fmt = thrdargs.logfmt;

    for (uint64_t i = 0; i < NREPEATS; i += 1) {
        fscrb_writeln(st, fmt, "Thread %d writing to the log -- #%d", id, i);
    }

    return NULL;
}

int main(void)
{
#if defined(PTHREAD)
    pthread_t threads[NTHREADS];
#endif
    struct thread_arg args[NTHREADS];

    if (SCRIBE_Success != scrb_init()) {
        exit (EXIT_FAILURE);
    }
    
    scrb_format const * const fmt = scrb_create_format("(%p|%F|%L|%M) %m");
    if (NULL == fmt) {
        exit (EXIT_FAILURE);
    }

    // use the file "synchtest.log" in synchronized write-only mode.
    scrb_stream const * const log = scrb_open_stream("synchtest.log", "w", true);
    if (NULL == log) {
        scrb_format_release(&fmt);
        exit (EXIT_FAILURE);
    }
    
    for (uint8_t th = 0; th < NTHREADS; th += 1) {
        args[th] = (struct thread_arg) { .thread_id = th, .logstream = log, .logfmt = fmt };
#if SCRIBE_DEBUG
        scrb_debug_write("Spawning thread #%d", th);
#endif
        pthread_create(&threads[th], NULL, threadlog, (void *) &args[th]); 
    }

    for (uint8_t th = 0; th < NTHREADS; th += 1) {
#if SCRIBE_DEBUG
        scrb_debug_write("Joining thread #%d", th);
#endif
        pthread_join(threads[th], NULL);
    }

    scrb_close_stream(&log);
    scrb_format_release(&fmt); 
    exit (EXIT_SUCCESS);
}

#if defined(PRTHREAD)
#undef PTHREAD
#endif
#undef NTHREADS
#undef NREPEATS

