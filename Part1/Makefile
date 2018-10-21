HDR:=$(UTIL)/varstring.h $(UTIL)/strcontains.h

all: $(BIN)/part1 $(BIN)/testvarstring
.PHONY: all

$(BIN)/part1: part1.c $(HDR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/testvarstring: testvarstring.c $(HDR)
	$(CC) $(CFLAGS) $< -o $@
