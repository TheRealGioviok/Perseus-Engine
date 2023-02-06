# This makefile is used to build the program.
# We will configure two targets: one for debug and one for release.
# The debug target will be used to build the program with debug symbols.
# The release target will be used to build the program without debug symbols.

# The name of the program
PROGRAM = Perseus

# The name of the compiler
CC = gcc

# The name of the linker
LD = gcc

# Language standard
STD = -std=c++17

# Files to be compiled
SOURCES = $(wildcard *.cpp)
# Main file
MAIN = Perseus.cpp
# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Target 0: all. This target will be used to build the program.
all: debug

# Target 1: debug. This target will be used to build the program with debug symbols.
debug: CFLAGS = -g -Wall -Wextra -Wpedantic -Werror -O0
debug: $(PROGRAM)

# Target 2: release. This target will be used to build the program without debug symbols.
release: CFLAGS = -Wall -Wextra -Wpedantic -Werror -O3
release: $(PROGRAM)
