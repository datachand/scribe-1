//
// Test scribe initialization
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
#endif
    return EXIT_SUCCESS;
error:
    return EXIT_FAILURE;
}

