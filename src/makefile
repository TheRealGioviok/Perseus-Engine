# This makefile is used to build the program.
# We will configure two targets: one for debug and one for release.
# The debug target will be used to build the program with debug symbols.
# The release target will be used to build the program without debug symbols.

THIS := $(realpath $(dir $(abspath $(lastword $(MAKEFILE_LIST)))))
ROOT := $(realpath $(THIS)/..)
CXX := g++
TARGET:= Perseus
CXXFLAGS := -Wall -std=c++17
NATIVE := -march=native

NAME := $(TARGET)

# Detect OS
ifeq ($(OS),Windows_NT)
	OS := Windows_NT
	SUFFIX := .exe
else
	OS := $(shell uname -s)
	SUFFIX :=
endif

# I am too lazy to include apple support
SOURCES := $(wildcard *.cpp)
EXECUTABLE := $(NAME)$(SUFFIX)

.PHONY: all debug release

all: release

# Debug target
debug: CXXFLAGS += -g -O0 -fsanitize=address -fno-omit-frame-pointer -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
debug: $(EXECUTABLE)

# Release target
release: CXXFLAGS += -funroll-loops -O3 -flto -fno-exceptions -DNDEBUG -lm
release: $(EXECUTABLE)


# Build the program
$(EXECUTABLE): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(NATIVE) $(SOURCES) -o $@