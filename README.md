# scribe

## Description
A fast and simple stream based logging library for C. Note, most of the api is unstable at the moment.

Currently scribe is known to build successfully on OS X with clang-600.0.56 and gcc 4.9.2.

Author: Dalton Woodard

Contact: daltonmwoodard@gmail.com

## TODO
- Add asynchronous queuing for stream objects to reduce time spent in blocking IO calls.

- Add rule/level based logging option to broadcast messages to several streams at once (i.e., the syslog model).

## Benchmarks
All tests listed here were run on OS X 10.10.1 with 2.5 GHz 2-core Intel i5.

- Unsynchronized Throughput: 1,000,000 log messages written by a single thread (see `examples/throughput.c`):
```
$ ./examples/throughput
real    0m0.522s
user    0m0.402s
sys     0m0.099s
```

- Synchronized Throughput: 1,000,000 log messages written each by 4 threads to the same file (see
  `examples/synchronized.c`). Notice the sub-linear performance by increasing number of threads compared to above:
```
$ ./examples/synchronized
real    0m1.942s
user    0m5.739s
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


