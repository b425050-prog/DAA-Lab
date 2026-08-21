CC := gcc
CFLAGS := -std=c17 -O2 -Wall -Wextra -Wpedantic
STRICT_FLAGS := $(CFLAGS) -Werror
LDLIBS := -lm
BIN := bin

SOURCES := \
	Q-1/q1_stable_colour_sort.c \
	Q-1/q1_experimental_validation.c \
	Q-1/q1_plot_linear.c \
	Q-2/q2_cross_set_pair_sum.c \
	Q-2/q2_experimental_validation.c \
	Q-2/q2_plot_complexity.c \
	Q-3/q3_generalized_k_sum.c \
	Q-3/q3_experimental_validation.c \
	Q-3/q3_plot_complexity.c \
	Q-4/q4_party_peak.c \
	Q-4/q4_experimental_validation.c \
	Q-4/q4_plot_complexity.c \
	Q-5/q5_merge_intervals.c \
	Q-5/q5_experimental_validation.c \
	Q-5/q5_plot_complexity.c \
	Q-6/q6_max_interval_overlap.c \
	Q-6/q6_experimental_validation.c \
	Q-6/q6_plot_complexity.c

TARGETS := $(patsubst %.c,$(BIN)/%,$(SOURCES))

.PHONY: all evidence strict clean

all: $(TARGETS)

$(BIN)/%: %.c common/svg_plot.h
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

evidence: all
	@cd Q-1 && ../../lab4/$(BIN)/Q-1/q1_experimental_validation > q1_experiment_output.txt
	@cd Q-1 && ../../lab4/$(BIN)/Q-1/q1_plot_linear
	@cd Q-2 && ../../lab4/$(BIN)/Q-2/q2_experimental_validation > q2_experiment_output.txt
	@cd Q-2 && ../../lab4/$(BIN)/Q-2/q2_plot_complexity
	@cd Q-3 && ../../lab4/$(BIN)/Q-3/q3_experimental_validation > q3_experiment_output.txt
	@cd Q-3 && ../../lab4/$(BIN)/Q-3/q3_plot_complexity
	@cd Q-4 && ../../lab4/$(BIN)/Q-4/q4_experimental_validation > q4_experiment_output.txt
	@cd Q-4 && ../../lab4/$(BIN)/Q-4/q4_plot_complexity
	@cd Q-5 && ../../lab4/$(BIN)/Q-5/q5_experimental_validation > q5_experiment_output.txt
	@cd Q-5 && ../../lab4/$(BIN)/Q-5/q5_plot_complexity
	@cd Q-6 && ../../lab4/$(BIN)/Q-6/q6_experimental_validation > q6_experiment_output.txt
	@cd Q-6 && ../../lab4/$(BIN)/Q-6/q6_plot_complexity

strict:
	@set -e; for source in $(SOURCES); do \
		$(CC) $(STRICT_FLAGS) "$$source" $(LDLIBS) -o /tmp/daa_lab4_strict; \
		echo "PASS  $$source"; \
	done

clean:
	rm -rf $(BIN)
