#
# test dir level (/scribe/test/) makefile for scribe
#

base=..
self=.
build_dir=$(base)/build
include_dir=$(base)/include
submodules_dir=$(base)/submodules

sources=$(wildcard *.c)
dep=$(build_dir)/libscribe.a
targets=$(basename $(sources))

CC?=clang
std?=c11
o_flags?=-O2
dbg_flags?=-g0
cflags=-std=$(std) $(o_flags) $(dbg_flags) $(OPTFLAGS) -Werror -Wall -Wextra \
	   -Wformat=2 -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition \
	   -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wcast-align \
	   -Wmissing-include-dirs -Wswitch-default -Wno-unused-function -Wno-format-nonliteral \
	   -Wno-gnu-zero-variadic-macro-arguments
iflags=-I$(include_dir) -I$(submodules_dir)/spinlock/include
lflags=-L$(biuld_dir)

ifeq ($(findstring gcc,$(CC)),gcc)
	cflags:=$(filter-out -Wno-gnu-zero-variadic-macro-arguments, $(cflags))
endif

.PHONY: all

all: $(targets)

$(targets): $(sources)
	$(foreach tgt, $(targets), $(CC) $(iflags) $(cflags) $(tgt).c -o $(tgt) $(dep); )

clean:
	@rm -rf $(targets) *.dSYM *.log

