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

static
void writetime(char ** buff, size_t * len, SCRIBE_TIME_T ts)
{
    *buff = NULL;
    *len = 0;
    (void) ts;
/*    // length of the final time string we produce
#define TIMELEN 25
    char * writedst;

    // if the buffer size given to us by the library is
    // smaller than we need, we'll use the given `buff`
    // pointer and allocate enough room.
    if (TIMELEN > *maxlen) {
        *buff = malloc(TIMELEN * sizeof(char));
        if (NULL == *buff) {
            goto error;
        }
    }

    writedst = *buff;
#if defined(SCRIBE_WINDOWS)
    snprinf(writedst, TIMELEN, "%04d:%02d:%02d:%02d:%02d:%02d.%03d",
            ts.wYear, ts.wMonth, ts.wDay, ts.wHour, 
            ts.wMinute, ts.wSecond, ts.Milliseconds);
#else
    strftime(writedst, TIMELEN, "%G:%m:%d:%H:%M:%S.", localtime(&ts.tv_sec));
    snprintf(writedst + 20, 4, "%ld", (long int) ts.tv_usec);
#endif 
    *maxlen = TIMELEN;
    return (SCRIBE_Success);
error:
    return (SCRIBE_Failure);
#undef TIMELEN
*/
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
    
    scrb_format const * const fmt = scrb_create_format("(%F|%L|%M) %m", &writetime);
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

