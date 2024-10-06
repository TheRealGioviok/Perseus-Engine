# Define the executable name (match this with EXE in src Makefile)
EXE := Perseus

# Default target (release build)
all: release

# Release target
release:
	$(MAKE) -C src release EXE=$(EXE)
	ifeq ($(OS),Windows_NT)
		move src/$(EXE) .
	else
		mv src/$(EXE) .

# Debug target
debug:
	$(MAKE) -C src debug EXE=$(EXE)
	ifeq ($(OS),Windows_NT)
		move src/$(EXE) .
	else
		mv src/$(EXE) .

# Clean target
clean:
	$(MAKE) -C src clean EXE=$(EXE)
	ifeq ($(OS),Windows_NT)
		move src/$(EXE) .
	else
		mv src/$(EXE) .
