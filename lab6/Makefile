CC := gcc
CFLAGS := -std=c17 -O2 -Wall -Wextra -Wpedantic
STRICT_FLAGS := $(CFLAGS) -Werror
LDLIBS := -lm
BIN := bin

SOURCES := \
	Q-1/q1_array_operations.c Q-1/q1_experimental_validation.c Q-1/q1_plot_complexity.c \
	Q-2/q2_matrix_operations.c Q-2/q2_experimental_validation.c Q-2/q2_plot_complexity.c \
	Q-3/q3_fft_convolution.c Q-3/q3_experimental_validation.c Q-3/q3_plot_complexity.c \
	Q-4/q4_reversal_sort.c Q-4/q4_experimental_validation.c Q-4/q4_plot_complexity.c \
	Q-5/q5_fibonacci_dp.c Q-5/q5_experimental_validation.c Q-5/q5_plot_complexity.c \
	Q-6/q6_knapsack_dp.c Q-6/q6_experimental_validation.c Q-6/q6_plot_complexity.c \
	Q-7/q7_lcs_dp.c Q-7/q7_experimental_validation.c Q-7/q7_plot_complexity.c \
	Q-8/q8_matrix_chain_dp.c Q-8/q8_experimental_validation.c Q-8/q8_plot_complexity.c

TARGETS := $(patsubst %.c,$(BIN)/%,$(SOURCES))

.PHONY: all evidence strict clean

all: $(TARGETS)

$(BIN)/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

evidence: all
	@set -e; for q in 1 2 3 4 5 6 7 8; do \
		cd Q-$$q; ../$(BIN)/Q-$$q/q$${q}_experimental_validation > q$${q}_experiment_output.txt; \
		../$(BIN)/Q-$$q/q$${q}_plot_complexity; cd ..; \
	done

strict:
	@mkdir -p $(BIN)
	@set -e; for source in $(SOURCES); do \
		$(CC) $(STRICT_FLAGS) "$$source" $(LDLIBS) -o $(BIN)/strict_check; \
		echo "PASS  $$source"; \
	done
	@rm -f $(BIN)/strict_check

clean:
	rm -rf $(BIN)
