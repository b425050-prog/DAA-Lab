CC ?= gcc
CFLAGS ?= -std=c11 -O2 -Wall -Wextra -pedantic
LDFLAGS ?=

PROGRAMS = \
	lab1/Q-1/q1_growth_order.exe \
	lab1/Q-2/q2_coin_toss.exe \
	lab1/Q-3/q3_bubble_sort.exe \
	lab1/Q-4/q4_towers_of_hanoi.exe \
	lab1/Q-5/q5_partition_point.exe \
	lab1/Q-6/q6_element_uniqueness.exe

all: $(PROGRAMS)

lab1/Q-1/q1_growth_order.exe: lab1/Q-1/q1_growth_order.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -lm

lab1/Q-2/q2_coin_toss.exe: lab1/Q-2/q2_coin_toss.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@ -lm

lab1/Q-3/q3_bubble_sort.exe: lab1/Q-3/q3_bubble_sort.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

lab1/Q-4/q4_towers_of_hanoi.exe: lab1/Q-4/q4_towers_of_hanoi.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

lab1/Q-5/q5_partition_point.exe: lab1/Q-5/q5_partition_point.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

lab1/Q-6/q6_element_uniqueness.exe: lab1/Q-6/q6_element_uniqueness.c
	$(CC) $(CFLAGS) $(LDFLAGS) $< -o $@

clean:
	rm -f $(PROGRAMS)

.PHONY: all clean
