CC := gcc
CFLAGS := -std=c17 -O2 -Wall -Wextra -Wpedantic
BIN := bin
PYTHON ?= python3

SOURCES := \
	Q-1/q1_coin_triangle.c \
	Q-2/q2_egg_dropping_dp.c \
	Q-3/q3_reves_puzzle.c \
	Q-4/q4_security_switches.c \
	Q-5/q5_moving_target.c \
	Q-6/q6_best_time_alive.c \
	Q-7/q7_matrix_chain_dp.c
TARGETS := $(patsubst %.c,$(BIN)/%,$(SOURCES))

.PHONY: all evidence test strict clean
all: $(TARGETS)

$(BIN)/%: %.c common/lab7_common.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -o $@

test:
	CC="$(CC)" $(PYTHON) tools/test_all.py

evidence: test
	$(PYTHON) tools/capture_samples.py

strict:
	@mkdir -p $(BIN)
	@set -e; for source in $(SOURCES); do \
		$(CC) $(CFLAGS) -Werror "$$source" -o $(BIN)/strict_check; \
		echo "PASS  $$source"; \
	done
	@rm -f $(BIN)/strict_check

clean:
	rm -rf $(BIN)
