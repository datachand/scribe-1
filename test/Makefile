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
target=

CC?=clang
std?=c11
opt_flags?=-O2
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

all: $(target)

$(target): $(source)
	$(CC) $(iflags) $(cflags) $^ -o $@ $(dep)

clean:
	@rm -rf $(target) *.dSYM

