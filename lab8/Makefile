CC ?= gcc
CFLAGS ?= -std=c17 -O2 -Wall -Wextra -Wpedantic -Werror
PYTHON ?= python3
SOURCES := Q-1/q1_minimum_coin_change.c Q-1/q1_plot_evidence.c Q-2/q2_coin_change_ways.c Q-2/q2_plot_evidence.c Q-3/q3_lcs_traceback.c Q-3/q3_plot_evidence.c Q-4/q4_longest_increasing_subsequence.c Q-4/q4_plot_evidence.c Q-5/q5_maximum_sum_increasing_subsequence.c Q-5/q5_plot_evidence.c Q-6/q6_edit_distance_traceback.c Q-6/q6_plot_evidence.c Q-7/q7_rod_cutting_reconstruction.c Q-7/q7_plot_evidence.c Q-8/q8_optimal_binary_search_tree.c Q-8/q8_plot_evidence.c Q-9/q9_collatz_analyser.c Q-9/q9_plot_evidence.c
TARGETS := $(patsubst %.c,bin/%,$(SOURCES))
.PHONY: all strict test evidence visuals clean
all: $(TARGETS)
bin/%: %.c common/lab8_common.h common/plot_from_dat.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< -lm -o $@
strict:
	CC="$(CC)" $(PYTHON) tools/build.py
test evidence:
	CC="$(CC)" $(PYTHON) tools/validate.py
visuals:
	$(PYTHON) tools/generate_visuals.py
clean:
	rm -rf bin
