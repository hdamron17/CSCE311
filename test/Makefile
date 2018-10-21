TEST:=./test.sh
PARTS:=part1 part2 part3
EXECS:=./sample.py
EXECS+=$(foreach part, $(PARTS), $(BIN)/$(part))

# TODO Implicitly depends on $(EXECS) but cannot report that dependency
test:
	@echo "======"
	@$(foreach exec, $(EXECS), $(TEST) $(exec) &&) true

.PHONY: all test_py test_part1
