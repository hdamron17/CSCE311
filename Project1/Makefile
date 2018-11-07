BIN:=$(CURDIR)/bin
INCLUDE:=$(CURDIR)/include
UTIL:=$(INCLUDE)/util
SRC:=$(CURDIR)/src
PARTS:=Part1 Part2 Part3
MKDIRS:=bin
CFLAGS:=-I$(INCLUDE) -Wall -Wextra -pedantic -Werror=deprecated-declarations -g
UNAME:=$(shell uname)
BSIZE:=20

TEST:=$(CURDIR)/test
TESTPROG:=$(TEST)/test.sh
EXECS:=$(TEST)/sample.py $(foreach part, $(PARTS), $(BIN)/$(part))

ifeq ($(UNAME), Darwin)  # MacOS doesn't need -lrt
  LRT=
else
  LRT=-lrt
endif

all: $(PARTS)

HDR=$(UTIL)/strcontains.h $(UTIL)/varstring.h

Part1: $(BIN)/Part1
$(BIN)/Part1: $(SRC)/Parts12.c $(HDR) | $(MKDIRS)
	$(CC) $(CFLAGS) -DPART1 -BSIZE=$(BSIZE) $< -o $@

Part2: $(BIN)/Part2
$(BIN)/Part2: $(SRC)/Parts12.c $(HDR) | $(MKDIRS)
	$(CC) $(CFLAGS) -DPART2 -BSIZE=$(BSIZE) $< -o $@

Part3: $(BIN)/Part3
$(BIN)/Part3: $(SRC)/Part3.c $(HDR) | $(MKDIRS)
	$(CC) $(CFLAGS) -pthread $< $(LRT) -o $@

$(MKDIRS):
	@mkdir -p $(MKDIRS)

clean:
	@rm -rf $(MKDIRS)

test:
	@echo "======"
	@cd $(TEST); $(foreach exec, $(EXECS), $(TESTPROG) $(exec) &&) true

.PHONY: all clean $(PARTS) test
