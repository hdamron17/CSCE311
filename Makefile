BIN:=$(CURDIR)/bin
SUBDIRS:=Part1 #Part2 Part3

export BUILD BIN

all: mkdirs
	$(foreach subdir, $(SUBDIRS), make -C $(subdir))

mkdirs:
	@mkdir -p $(BIN)

clean:
	@rm -rf $(BIN)

.PHONY: all mkdirs clean
