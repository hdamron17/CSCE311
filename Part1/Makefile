SRC:=part1.c
HDR:=$(UTIL)/varstring.h
DEST:=$(BIN)/part1

all: $(DEST) $(BIN)/testvarstring
.PHONY: all

$(DEST): $(SRC) $(HDR)
	$(CC) $(CFLAGS) $< -o $@

$(BIN)/testvarstring: testvarstring.c $(HDR)
	$(CC) $(CFLAGS) $< -o $@
