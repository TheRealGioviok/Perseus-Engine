# Define the executable name (match this with EXE in src Makefile)
EXE := Perseus

# Detect the operating system
ifeq ($(OS),Windows_NT)
    MOVE_CMD := move
else
    MOVE_CMD := mv
endif

# Default target (release build)
all: 
	$(MAKE) -C src EXE=$(EXE)
	$(MOVE_CMD) src/$(EXE) .

# Release target
release:
	$(MAKE) -C src release EXE=$(EXE)
	$(MOVE_CMD) src/$(EXE) .

# Debug target
debug:
	$(MAKE) -C src debug EXE=$(EXE)
	$(MOVE_CMD) src/$(EXE) .

# Clean target
clean:
	$(MAKE) -C src clean EXE=$(EXE)
