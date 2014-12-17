#!/bin/bash

# script for travis-ci builds using clang
echo "clang c11 debug build";
make std=c11;
./examples/throughput;
tail scribedebug.log;
make clean;

echo "clang c11 regular build";
make std=c11 OPTFLAGS=-DNDEBUG;
./examples/throughput;
make clean;

echo "clang c99 debug build";
make std=c99;
./examples/throughput;
tail scribedebug.log;
make clean;

echo "clang c99 regular build";
make std=c99 OPTFLAGS=-DNDEBUG;
./examples/throughput;
make clean;

exit 0;

