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
    scrb_format const * const fmt = scrb_create_format("%M");
    scrb_format_release(&fmt); 
    exit (EXIT_SUCCESS);
}

