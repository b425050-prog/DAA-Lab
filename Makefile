CC := gcc
CFLAGS := -std=c17 -O2 -Wall -Wextra -pedantic
LDLIBS := -lm
BIN := bin

SOURCES := \
	Q-1/binary_ternary_search.c \
	Q-1/binary_ternary_generate_data.c \
	Q-2/defective_coin.c \
	Q-2/defective_coin_generate_data.c \
	Q-3/max_min_dc.c \
	Q-3/max_min_generate_data.c \
	Q-4/strassen.c \
	Q-4/strassen_generate_data.c \
	Q-5/special_matrix_dc.c \
	Q-5/special_matrix_generate_data.c \
	Q-6/selection_sort_invariant.c \
	Q-6/selection_sort_generate_data.c

TARGETS := $(patsubst %.c,$(BIN)/%,$(SOURCES))

.PHONY: all clean plots

all: $(TARGETS)

$(BIN)/%: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $< $(LDLIBS) -o $@

plots:
	./scripts/regenerate_all.sh

clean:
	rm -rf $(BIN)
	find . -name '*.dat' -delete
