//
// High throughput logging example for scribe usage.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <stdint.h>
#include <stdio.h>
#include "scribe.h"

#define REPEATS 1000000

int main(void)
{
    if (SCRIBE_Success != scrb_init()) {
        exit (EXIT_FAILURE);
    }
    
    scrb_format const * const fmt = scrb_create_format("(%p|%F|%L|%M) %m");
    if (NULL == fmt) {
        exit (EXIT_FAILURE);
    }

    // use the file "test.log" in write-only mode unsynchronized without mmap usage.
    scrb_stream const * const log = scrb_open_stream("test.log", "a", false, false);
    if (NULL == log) {
        scrb_format_release(&fmt);
        exit (EXIT_FAILURE);
    }
    
    for (uint64_t i = 0; i < REPEATS; i += 1) {
        scrb_writeln(log, fmt, "This writes to the log file.");
    }

    scrb_close_stream(&log);
    scrb_format_release(&fmt); 
    exit (EXIT_SUCCESS);
}

