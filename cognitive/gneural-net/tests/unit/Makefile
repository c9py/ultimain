# Makefile for gneural_network unit tests

CC = gcc
CFLAGS = -Wall -Wextra -g -I../../include
LDFLAGS = -lm

# Source directories
SRC_DIR = ../../src
TEST_DIR = .

# Test sources
TEST_SRCS = test_runner.c test_math_utils.c test_activation.c test_network.c test_feedforward.c

# Library sources needed for tests (compiled from main source)
LIB_SRCS = $(SRC_DIR)/fact.c \
           $(SRC_DIR)/binom.c \
           $(SRC_DIR)/rnd.c \
           $(SRC_DIR)/activation.c \
           $(SRC_DIR)/network.c \
           $(SRC_DIR)/feedforward.c \
           $(SRC_DIR)/simulated_annealing.c \
           $(SRC_DIR)/random_search.c \
           $(SRC_DIR)/gradient_descent.c \
           $(SRC_DIR)/genetic_algorithm.c \
           $(SRC_DIR)/msmco.c \
           $(SRC_DIR)/randomize.c \
           $(SRC_DIR)/error.c

# Object files
TEST_OBJS = $(TEST_SRCS:.c=.o)
LIB_OBJS = fact.o binom.o rnd.o activation.o network.o feedforward.o \
           simulated_annealing.o random_search.o gradient_descent.o \
           genetic_algorithm.o msmco.o randomize.o error.o

# Test executable
TEST_BIN = test_runner

.PHONY: all clean run test

all: $(TEST_BIN)

$(TEST_BIN): $(TEST_OBJS) $(LIB_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile test files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Compile library files from src/
fact.o: $(SRC_DIR)/fact.c
	$(CC) $(CFLAGS) -c $< -o $@

binom.o: $(SRC_DIR)/binom.c
	$(CC) $(CFLAGS) -c $< -o $@

rnd.o: $(SRC_DIR)/rnd.c
	$(CC) $(CFLAGS) -c $< -o $@

activation.o: $(SRC_DIR)/activation.c
	$(CC) $(CFLAGS) -c $< -o $@

network.o: $(SRC_DIR)/network.c
	$(CC) $(CFLAGS) -c $< -o $@

feedforward.o: $(SRC_DIR)/feedforward.c
	$(CC) $(CFLAGS) -c $< -o $@

simulated_annealing.o: $(SRC_DIR)/simulated_annealing.c
	$(CC) $(CFLAGS) -c $< -o $@

random_search.o: $(SRC_DIR)/random_search.c
	$(CC) $(CFLAGS) -c $< -o $@

gradient_descent.o: $(SRC_DIR)/gradient_descent.c
	$(CC) $(CFLAGS) -c $< -o $@

genetic_algorithm.o: $(SRC_DIR)/genetic_algorithm.c
	$(CC) $(CFLAGS) -c $< -o $@

msmco.o: $(SRC_DIR)/msmco.c
	$(CC) $(CFLAGS) -c $< -o $@

randomize.o: $(SRC_DIR)/randomize.c
	$(CC) $(CFLAGS) -c $< -o $@

error.o: $(SRC_DIR)/error.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TEST_BIN)
	./$(TEST_BIN)

test: run

clean:
	rm -f $(TEST_BIN) *.o
