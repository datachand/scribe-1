#
# examples dir level (/scribe/examples/) makefile for scribe
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
std=c11
opt_flags?=-O2
dbg_flags?=-g0
cflags=-std=$(std) $(opt_flags) $(dbg_flags) -Werror -Wall -Wextra -Wformat=2 -Wshadow \
	   -Wwrite-strings -Wstrict-prototypes -Wold-style-definition -Wredundant-decls \
	   -Wnested-externs -Wmissing-include-dirs -Wcast-align -Wmissing-include-dirs \
	   -Wswitch-default -Wno-unused-function -Wno-format-nonliteral
iflags=-I$(include_dir) -I$(submodules_dir)/spinlock/include
lflags=-L$(biuld_dir)

.PHONY: all

all: $(targets)

$(targets): $(sources)
	$(foreach tgt, $(targets), $(CC) $(iflags) $(cflags) $(tgt).c -o $(tgt) $(dep); )

clean:
	@rm -rf $(targets) *.dSYM *.log

