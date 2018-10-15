BIN:=$(CURDIR)/bin
INCLUDE:=$(CURDIR)/include
UTIL:=$(INCLUDE)/util
SUBDIRS:=Part1 Part2 Part3
MKDIRS:=bin
CFLAGS:=-I$(INCLUDE)

export BIN UTIL CFLAGS

all: $(SUBDIRS)
$(SUBDIRS): | $(MKDIRS)
	make -C $@

$(MKDIRS):
	@mkdir -p $(MKDIRS)

clean:
	@rm -rf $(MKDIRS)

.PHONY: all clean $(SUBDIRS)
