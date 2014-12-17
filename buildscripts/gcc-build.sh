#!/bin/bash

# script for travis-ci builds using gcc

echo "gcc c99 debug build";
make std=c99;
./examples/throughput;
tail scribedebug.log;
make clean;

echo "gcc c99 regular build";
make std=c99 OPTFLAGS=-DNDEBUG;
./examples/throughput;
make clean;

exit 0;

