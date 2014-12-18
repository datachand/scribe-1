//
// Test output to standard streams
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
    if (SCRB_Success != scrb_init()) {
        goto error;
    }
    
    if (NULL == scrb_stdout || NULL == scrb_stdout->name || stdout != scrb_stdout->stream.filestream) {
#if SCRIBE_DEBUG
        scrb_debug_write("stdout initialization failed.");
#endif
        goto error;
    }

    if (NULL == scrb_stdin || NULL == scrb_stdin->name || stdin != scrb_stdin->stream.filestream) {
#if SCRIBE_DEBUG
        scrb_debug_write("stdin initialization failed.");
#endif
       goto error;
    }
    
    if (NULL == scrb_stderr || NULL == scrb_stderr->name || stderr != scrb_stderr->stream.filestream) {
#if SCRIBE_DEBUG
        scrb_debug_write("stderr initialization failed.");
#endif
       goto error;
    }

#if SCRIBE_DEBUG
    scrb_debug_write("std streams initialized successfully.");
    scrb_debug_write("now testing output to stdout and stderr.");
#endif

    if (SCRB_Success != scrb_putstrln(scrb_stdout, "writing to stdout.")) {
#if SCRIBE_DEBUG
        scrb_debug_write("write to stdout failed.");
#endif
        goto error;
    }

    if (SCRB_Success != scrb_putstrln(scrb_stderr, "writing to stderr.")) {
#if SCRIBE_DEBUG
        scrb_debug_write("write to stderr failed.");
#endif
        goto error;
    }

#if SCRIBE_DEBUG
    scrb_debug_write("writing to std streams completed successfully.");
#endif
    return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

