//
// Test program for scribe
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#include <stdio.h>
#include <stdlib.h>

#include "scribe.h"

int main(void)
{
    if (SCRIBE_Success != scrb_init()) {
        exit (EXIT_FAILURE);
    }
    scrb_format const * const fmt = scrb_create_format("(%p|%f|%l|%m) %M");
    fprintf(stderr, "%s\n", fmt->fmtleader);
    uint64_t i;
    for (i = 0; i < fmt->nsplits; i += 1) {
        fprintf(stderr, "%s\n", fmt->fmtstr_split[i].string);
    }
    scrb_writeln(scrb_stdout, fmt, "Hello from scribe!");
    fscrb_writeln(scrb_stdout, fmt, "This is %s", "formatted!");
    
    scrb_stream const * const log = scrb_open_stream("test.log", "w", false);
    scrb_writeln(log, fmt, "This writes to the log file!"); 
    scrb_close_stream(&log);
    scrb_format_release(&fmt); 
    exit (EXIT_SUCCESS);
}

