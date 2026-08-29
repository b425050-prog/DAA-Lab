CC := gcc
CFLAGS := -std=c17 -O2 -Wall -Wextra -Wpedantic
STRICT_FLAGS := $(CFLAGS) -Werror
LDLIBS := -lm
BIN := bin

SOURCES := \
	Q-1/q1_median_bfprt.c \
	Q-1/q1_experimental_validation.c \
	Q-1/q1_plot_complexity.c \
	Q-2/q2_kth_smallest.c \
	Q-2/q2_experimental_validation.c \
	Q-2/q2_plot_complexity.c \
	Q-3/q3_random_file_quicksort.c \
	Q-3/q3_experimental_validation.c \
	Q-3/q3_plot_complexity.c \
	Q-4/q4_heap_sort_file.c \
	Q-4/q4_experimental_validation.c \
	Q-4/q4_plot_complexity.c

HEADERS := common/svg_plot.h Q-2/q2_bfprt.h Q-3/q3_quicksort.h
TARGETS := $(patsubst %.c,$(BIN)/%,$(SOURCES))

.PHONY: all evidence strict clean

all: $(TARGETS)

$(BIN)/%: %.c $(HEADERS)
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

evidence: all
	@cd Q-1 && ../$(BIN)/Q-1/q1_experimental_validation > q1_experiment_output.txt
	@cd Q-1 && ../$(BIN)/Q-1/q1_plot_complexity
	@cd Q-2 && ../$(BIN)/Q-2/q2_experimental_validation > q2_experiment_output.txt
	@cd Q-2 && ../$(BIN)/Q-2/q2_plot_complexity
	@cd Q-3 && ../$(BIN)/Q-3/q3_experimental_validation > q3_experiment_output.txt
	@cd Q-3 && ../$(BIN)/Q-3/q3_plot_complexity
	@cd Q-4 && ../$(BIN)/Q-4/q4_experimental_validation > q4_experiment_output.txt
	@cd Q-4 && ../$(BIN)/Q-4/q4_plot_complexity

strict:
	@mkdir -p $(BIN)
	@set -e; for source in $(SOURCES); do \
		$(CC) $(STRICT_FLAGS) "$$source" $(LDLIBS) -o $(BIN)/strict_check; \
		echo "PASS  $$source"; \
	done
	@rm -f $(BIN)/strict_check

clean:
	rm -rf $(BIN)
