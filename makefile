EXE := Perseus
# Detect the operating system
ifeq ($(OS),Windows_NT)
	EXE_EXT := .exe
    MOVE_CMD := move
	PATH_SEPARATOR := \$()
else
	EXE :=
    MOVE_CMD := mv
	PATH_SEPARATOR := /
endif

ifdef CPU
	SUFX := _$(CPU)
else
	SUFX :=
endif

# Default target (release build)
all: release

# Release target
release:
	$(MAKE) -C src release
	@$(MOVE_CMD) src$(PATH_SEPARATOR)$(EXE)$(EXE_EXT) $(EXE)$(SUFX)$(EXE_EXT)

# Debug target
debug:
	$(MAKE) -C src debug
	@$(MOVE_CMD) src$(PATH_SEPARATOR)$(EXE)$(EXE_EXT) $(EXE)$(SUFX)$(EXE_EXT)

# Clean target
clean:
	$(MAKE) -C src clean
