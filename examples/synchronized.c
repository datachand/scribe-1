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
#define Pthread
#endif

#define NthreadS 4
#define NREPEATS 1000000

struct thread_arg {
    uint8_t const thread_id;
    struct scrb_stream * const logstream;
    struct scrb_format const * const logfmt;
};

static
void * threadlog(void * arg)
{
    struct thread_arg const thrdargs = *(struct thread_arg *) arg;
    
    uint8_t const id                     = thrdargs.thread_id;
    struct scrb_stream * const st        = thrdargs.logstream;
    struct scrb_format const * const fmt = thrdargs.logfmt;

    (void) id;
    (void) fmt;
    for (uint64_t i = 0; i < NREPEATS; i += 1) {
        //fscrb_writeln(st, fmt, "thread %d writing to the log -- #%d", id, i);
        scrb_writeln(st, fmt, "thread writing to the log");
    }

    return NULL;
}

static
void writetime(char ** buff, size_t * len, SCRIBE_TIME_T ts)
{
    *buff = NULL;
    *len  = 0;
    (void) ts;
}

int main(void)
{
#if defined(Pthread)
    pthread_t threads[NthreadS];
#endif
    struct thread_arg args[NthreadS];

    if (SCRB_Success != scrb_init()) {
        exit (EXIT_FAILURE);
    }
    
    struct scrb_format const * const fmt = scrb_create_format("(%F|%L|%M) %m", &writetime);
    if (NULL == fmt) {
        exit (EXIT_FAILURE);
    }

    // use the file "synchtest.log" in synchronized write-only mode with asynchronous IO on.
    struct scrb_stream * const log = scrb_open_stream("synchtest.log", "w", true, true);
    if (NULL == log) {
        scrb_format_release(&fmt);
        exit (EXIT_FAILURE);
    }
    
    for (uint8_t th = 0; th < NthreadS; th += 1) {
        struct thread_arg tmp = { .thread_id = th, .logstream = log, .logfmt = fmt };
        memcpy(args + th, &tmp, sizeof(struct thread_arg));
#if SCRIBE_DEBUG
        scrb_debug_write("Spawning thread #%d", th);
#endif
        pthread_create(&threads[th], NULL, threadlog, (void *) &args[th]); 
    }

    for (uint8_t th = 0; th < NthreadS; th += 1) {
#if SCRIBE_DEBUG
        scrb_debug_write("Joining thread #%d", th);
#endif
        pthread_join(threads[th], NULL);
    }

    scrb_close_stream(&log);
    scrb_format_release(&fmt); 
    exit (EXIT_SUCCESS);
}

#if defined(PRthread)
#undef Pthread
#endif
#undef NthreadS
#undef NREPEATS

