#
# test dir level (/scribe/test/) makefile for scribe
#

base=..
self=.
build_dir=$(base)/build
include_dir=$(base)/include
submodules_dir=$(base)/submodules

source=scribe_test.c
dep=$(build_dir)/libscribe.a
target=scribe_test

CC?=clang
std=c11
opt_flags=-O2
cflags=-std=$(std) $(opt_flags) -Werror -Wall -Wextra -Wformat=2 -Wshadow \
	   -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls \
	   -Wnested-externs -Wmissing-include-dirs -Wcast-align -Wmissing-include-dirs \
	   -Wswitch-default -Wno-unused-function -Wno-gnu-zero-variadic-macro-arguments \
	   -Wno-format-nonliteral
iflags=-I$(include_dir) -I$(submodules_dir)/spinlock/include
lflags=-L$(biuld_dir)

all: $(target)

$(target): $(source)
	$(CC) $(iflags) $(cflags) $^ -o $@ $(dep)

clean:
	@rm -rf $(target) *.dSYM

