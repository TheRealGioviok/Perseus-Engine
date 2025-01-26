# Detect the operating system
ifeq ($(OS),Windows_NT)
	EXE_EXT := .exe
    MOVE_CMD := move
	PATH_SEPARATOR := \$()
else
	EXE_EXT :=
    MOVE_CMD := mv
	PATH_SEPARATOR := /
endif

ifdef CPU
	SUFX := _$(CPU)
else
	SUFX :=
endif

EXE := Perseus

# Default target (release build)
all: release

# Release target
release:
	$(MAKE) -C src release EXE=$(EXE)$(EXE_EXT)
	@$(MOVE_CMD) src$(PATH_SEPARATOR)$(EXE)$(EXE_EXT) $(EXE)$(SUFX)$(EXE_EXT)

# Debug target
debug:
	$(MAKE) -C src debug EXE=$(EXE)$(EXE_EXT)
	@$(MOVE_CMD) src$(PATH_SEPARATOR)$(EXE)$(EXE_EXT) $(EXE)$(SUFX)$(EXE_EXT)

# Clean target
clean:
	$(MAKE) -C src clean EXE=$(EXE)$(EXE_EXT)
