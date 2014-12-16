# scribe 
[![Build Status](https://travis-ci.org/daltonwoodard/scribe.svg?branch=master)](https://travis-ci.org/daltonwoodard/sorting)

## Description
A fast and simple stream based logging library for C. The API should be mostly stable from now on. The current version
of scribe is v0.1.0.

Author: Dalton Woodard

Contact: daltonmwoodard@gmail.com

## Info
Currently scribe is known to build successfully on OS X with clang-600.0.56 and gcc 4.9.2 against the c11 standard.

Running `make` at the command line defaults to producing a static library `build/libscribe.a`. I won't be providing a
`make install` option or a dynamic (`.so`) library until the project is stable. See `Makefile` for more details.

If you wish to clone the project, make sure to run `git clone --recursive` in order to pull in the `spinlock.h` header
file as well.

## Todo
- Add asynchronous queuing for stream objects to reduce time spent in blocking IO calls.

- Add rule/level based logging option to broadcast messages to several streams at once (i.e., the syslog model).

## Benchmarks
All tests listed here were run on OS X 10.10.1 with 2.5 GHz 2-core Intel i5 and compiled with gcc 4.9.2.

- Unsynchronized Throughput: 1,000,000 log messages written by a single thread (see `examples/throughput.c`):
```
$ ./examples/throughput
real    0m0.463s
user    0m0.355s
sys     0m0.092s
```

- Synchronized Throughput: 1,000,000 log messages written each by 4 threads to the same file (see
  `examples/synchronized.c`).
```
$ ./examples/synchronized
real    0m1.941s
user    0m5.812s
sys     0m0.796s
```

## Example usage

```c
#include <stdio.h>
#include <stdlib.h>
#include "scribe.h"

int main(void)
{
    if (SCRB_Success != scrb_init()) {
        exit(EXIT_FAILURE);
    }

    // We make open a new file called `example.log` in write mode without synchronization.
    struct scrb_stream * const logstream = scrb_open_stream("example.log", "w", false);
    
    // We define a log format to tell scribe how to format our messages.
    // This format prints out the file, method, and line locations followed by the message itself.
    struct scrb_format * scrb_create_format("[%F:%L:%M] %m", NULL);

    // Now we're ready to go.
    scrb_logln(logstream, fmt, "Hello from the example program!");

    // We can do formatted output as well.
    scrb_flogln(logstream, fmt, "Hello for the %dnd time!", 2);

    // If we want to write directly to the file without formatting or location/time info
    // we can use the `scrb_putstrln` method.
    scrb_putstrln(logstream, "Now we're done.");

    scrb_format_release(fmt);
    scrb_close_stream(logstream);
    return 0;
}
```

Currently available log format information includes:

- File location
- Method location
- Line location
- Process ID
- Time (requires providing a callback to format time output)
- Stream name

## License
Copyright (c) 2014 Dalton M. Woodard

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit
persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the
Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR
OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


