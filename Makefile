#
# top level (/scribe/) makefile for scribe
# 

base=.
submodules_dir=$(base)/submodules
source_dir=$(base)/src
include_dir=$(base)/include
object_dir=$(base)/obj
build_dir=$(base)/build

sources=$(wildcard $(source_dir)/*.c)
objects=$(patsubst $(source_dir)%, $(object_dir)/%, $(sources:.c=.o))

CC?=clang
std=c11
opt_flags=-O2
cflags=-std=$(std) $(opt_flags) -Werror -Wall -Wextra -Wformat=2 -Wshadow \
	   -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls \
	   -Wnested-externs -Wmissing-include-dirs -Wcast-align -Wmissing-include-dirs \
	   -Wswitch-default -Wno-unused-function -Wno-gnu-zero-variadic-macro-arguments
iflags=-I$(include_dir)

.PHONY: all update setup build clean

all: build

setup:
	@mkdir -p $(object_dir) $(build_dir)

build: setup $(objects) $(target)

$(object_dir)/%.o: $(source_dir)/%.c
	$(CC) $(iflags) $(cflags) -c $^ -o $@

$(target) : $(objects)
	@ar rcs $@ $(objects) $(target_deps)
	@ranlib $@

clean:
	@rm -rf $(object_dir) $(build_dir) *.dSYM *.DS_Store

