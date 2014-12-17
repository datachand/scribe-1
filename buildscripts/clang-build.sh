#!/bin/bash

# script for travis-ci builds using clang

runprogs ()
{
    ./test/init;
    if [[ $? -ne 0 ]]; then echo "init failed"; return 1; fi;
    ./test/stdstreams;
    if [[ $? -ne 0 ]]; then echo "stdstreams failed"; return 1; fi;
    ./examples/throughput;
    if [[ $? -ne 0 ]]; then echo "throughput failed"; return 1; fi;
    ./examples/synchronized;
    if [[ $? -ne 0 ]]; then echo "synchronized failed"; return 1; fi;
    return 0;
}


echo "clang c11 debug build";
make std=c11;
if [[ runprogs -ne 0 ]]; then exit 1; fi;
tail scribedebug.log;
make clean;

echo "clang c11 regular build";
make std=c11 OPTFLAGS=-DNDEBUG;
if [[ runprogs -ne 0 ]]; then exit 1; fi;
make clean;

echo "clang c99 debug build";
make std=c99;
if [[ runprogs -ne 0 ]]; then exit 1; fi;
tail scribedebug.log;
make clean;

echo "clang c99 regular build";
make std=c99 OPTFLAGS=-DNDEBUG;
if [[ runprogs -ne 0 ]]; then exit 1; fi;
make clean;

exit 0;

