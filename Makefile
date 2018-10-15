BIN:=$(CURDIR)/bin
INCLUDE:=$(CURDIR)/include
UTIL:=$(INCLUDE)/util
SUBDIRS:=Part1 Part2 Part3
MKDIRS:=bin
CFLAGS:=-I$(INCLUDE)
SAMPLE:=$(BIN)/sample
UNAME:=$(shell uname)

ifeq ($(UNAME), Darwin)  # MacOS doesn't need -lrt
  LRT=
else
  LRT=-lrt
endif

export BIN UTIL CFLAGS UNAME LRT

all: $(SUBDIRS) sample
$(SUBDIRS): | $(MKDIRS)
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

.PHONY: all clean $(SUBDIRS) test
