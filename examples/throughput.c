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

#define NREPEATS 1000000

int main(void)
{
    if (SCRB_Success != scrb_init()) {
        exit (EXIT_FAILURE);
    }
    
    //struct scrb_format const * const fmt = scrb_create_format("(%t|%p|%F|%L|%M) %m", NULL);
    struct scrb_format const * const fmt = scrb_create_format("(%F|%L|%M) %m", NULL);
    if (NULL == fmt) {
        exit (EXIT_FAILURE);
    }

    // use the file "test.log" in write-only mode unsynchronized with buffered IO off.
    struct scrb_stream * const log = scrb_open_stream("throughputtest.log", "w", false, false);
    if (NULL == log) {
        scrb_format_release(&fmt);
        exit (EXIT_FAILURE);
    }
    (void) fmt; 
    for (uint64_t i = 0; i < NREPEATS; i += 1) {
        //fscrb_writeln(log, fmt, "This writes to the log file %s #%d", "test", i);
        //scrb_writeln(log, fmt, "This writes to the log file.");
        scrb_putstr(log, "This writes to the log file.");
    }

    scrb_close_stream(&log);
    scrb_format_release(&fmt); 
    exit (EXIT_SUCCESS);
}

