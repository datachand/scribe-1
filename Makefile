#
# top level (/scribe/) makefile for scribe
# 

base=.
submodules_dir=$(base)/submodules
source_dir=$(base)/src
include_dir=$(base)/include
object_dir=$(base)/obj
build_dir=$(base)/build
test_dir=$(base)/test
examples_dir=$(base)/examples

sources=$(wildcard $(source_dir)/*.c)
objects=$(patsubst $(source_dir)/%, $(object_dir)/%, $(sources:.c=.o))

target=$(build_dir)/libscribe.a

CC?=clang
std?=c11
o_flags?=-O2
dgb_flags?=-g0
cflags=-std=$(std) $(o_flags) $(dbg_flags) $(OPTFLAGS) -Werror -Wall -Wextra                 \
	   -Wformat=2 -Wshadow -Wwrite-strings -Wstrict-prototypes -Wold-style-definition        \
	   -Wredundant-decls -Wnested-externs -Wmissing-include-dirs -Wcast-align                \
	   -Wmissing-include-dirs -Wswitch-default -Wmissing-parameter-type -Wbad-function-cast  \
	   -Wmissing-prototypes -Wold-style-declaration -Wpointer-sign                           \
	   -Wpointer-arith -Wsequence-point  -Wsign-compare -Wsign-conversion -Wfloat-conversion \
	   -Wsizeof-pointer-memaccess -Wstack-protector -Wstrict-aliasing -Wuseless-cast         \
	   -Wvolatile-register-var -Wno-unused-function -Wno-format-nonliteral                   \
	   -Wno-gnu-zero-variadic-macro-arguments -Wno-unknown-warning-option
iflags=-I$(include_dir) -I$(submodules_dir)/spinlock/include

ifeq ($(findstring gcc,$(CC)),gcc)
	cflags:=$(filter-out -Wno-gnu-zero-variadic-macro-arguments, $(cflags))
	cflags:=$(filter-out -Wno-unkown-warning-option, $(cflags))
	cflags:=$(filter-out -Wuseless-cast, $(cflags))
endif

.PHONY: all update setup build test examples clean

all: examples test

update:
	git submodule foreach git pull origin master

setup:
	@mkdir -p $(object_dir) $(build_dir)

$(object_dir)/%.o: $(source_dir)/%.c
	$(CC) $(iflags) $(cflags) -c $^ -o $@

$(target) : $(objects)
	@ar rcs $@ $(objects) $(target_deps)
	@ranlib $@

build: setup $(objects) $(target)

test: build
	@make -C $(test_dir)

examples: build
	@make -C $(examples_dir)

clean:
	@rm -rf $(object_dir) $(build_dir) *.dSYM *.DS_Store
	@make -C $(test_dir) clean
	@make -C $(examples_dir) clean

