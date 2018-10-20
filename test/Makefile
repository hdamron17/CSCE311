TEST:=./test.sh
EXECS:=./sample.py $(BIN)/part1

# TODO Implicitly depends on $(EXECS) but cannot report that dependency
test:
	@echo "======"
	@$(foreach exec, $(EXECS), echo "Testing $(shell basename $(exec))"; echo "------"; $(TEST) $(exec); echo "======";)

.PHONY: all test_py test_part1
