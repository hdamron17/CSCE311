BIN:=$(CURDIR)/bin
INCLUDE:=$(CURDIR)/include
UTIL:=$(INCLUDE)/util
COMM:=$(CURDIR)/common
PARTS:=Part1 Part2 Part3
MKDIRS:=bin
CFLAGS:=-I$(INCLUDE) -Wall -Wextra -pedantic -Werror=deprecated-declarations -g
CXXFLAGS:=$(CFLAGS) -std=c++11
SAMPLE:=$(BIN)/sample
UNAME:=$(shell uname)

ifeq ($(UNAME), Darwin)  # MacOS doesn't need -lrt
  LRT=
else
  LRT=-lrt
endif

export BIN UTIL COMM CFLAGS CXXFLAGS UNAME LRT

all: $(PARTS) sample

Part1: $(BIN)/part1
$(BIN)/part1: $(COMM)/base.c $(UTIL)/strcontains.h $(UTIL)/varstring.h | $(MKDIRS)
	$(CC) $(CFLAGS) -DPART1 $< -o $@

Part2: $(BIN)/part2
$(BIN)/part2: $(COMM)/base.c $(UTIL)/strcontains.h $(UTIL)/varstring.h | $(MKDIRS)
	$(CC) $(CFLAGS) -DPART2 $< -o $@

Part3: | $(MKDIRS)
	make -C $@

sample: $(SAMPLE)
$(SAMPLE): readFileData.c
	$(CC) $(CFLAGS) $^ -o $@

$(MKDIRS):
	@mkdir -p $(MKDIRS)

clean:
	@rm -rf $(MKDIRS)

test:
	make -C test

.PHONY: all clean $(PARTS) test
