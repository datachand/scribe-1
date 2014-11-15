// 
// Format data type for internal usage.
//
// Author: Dalton Woodard
// Contact: daltonmwoodard@gmail.com
// License: Please see LICENSE.md
//

#ifndef SCRIBE_FORMAT_H
#define SCRIBE_FORMAT_H

#include <stdint.h>
#include <stdlib.h>

#include "scribe_conf.h"

typedef struct scrb_format scrb_format;

extern
scrb_format * scrb_create_format__internal(char const * const fmtstr);

extern
void scrb_format_release__internal(scrb_format ** fmtptr);

#endif

