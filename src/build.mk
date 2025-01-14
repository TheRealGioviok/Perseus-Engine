TARGET ?= Release
OBJ_DIR = obj/$(TARGET)

CPP_FILES := $(wildcard *.cpp)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(CPP_FILES:.cpp=.o))

CXX ?= clang++
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++20 -Wno-implicit-fallthrough
NATIVE := -march=native -mpopcnt
EXE ?= Perseus

PROFILE_DIR = profile-data
PROFRAW = $(PROFILE_DIR)/pgo.profraw
PROFDATA = $(PROFILE_DIR)/pgo.profdata

# Release target CXXFLAGS
ifeq ($(TARGET), Release)
	CXXFLAGS += -funroll-loops -O3 -flto -fno-exceptions -DNDEBUG -lm
else ifeq ($(TARGET), Debug)
	CXXFLAGS += -g -O0 -fsanitize=address -fno-omit-frame-pointer -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
else
	$(error "Unknown target: $(TARGET)")
endif

# Default target: full PGO process
.PHONY: all pgo-instrument pgo-run pgo-build clean release debug dirs link

all: clean pgo-instrument pgo-run pgo-build

# Build with instrumentation for PGO
pgo-instrument: dirs
	@echo "Building with instrumentation for PGO"
	@mkdir -p $(PROFILE_DIR)
	@for file in $(CPP_FILES); do \
	    obj_file=$(OBJ_DIR)/$$(basename $$file .cpp).o; \
	    $(CXX) $(CXXFLAGS) $(NATIVE) -fprofile-instr-generate=$(PROFRAW) -c $$file -o $$obj_file; \
	done
	@$(CXX) $(CXXFLAGS) $(NATIVE) -fprofile-instr-generate=$(PROFRAW) $(OBJ_FILES) -o $(EXE)


# Run the instrumented executable to collect profile data
pgo-run:
	@echo "Running instrumented executable to collect profile data"
	@mkdir -p $(PROFILE_DIR)
	@./$(EXE) bench 18

# Build optimized executable using collected profile data
pgo-build:
	@echo "Converting raw profile data to .profdata"
	@llvm-profdata merge -output=$(PROFDATA) $(PROFRAW)
	@echo "Building with PGO optimizations"
	@for file in $(CPP_FILES); do \
	    obj_file=$(OBJ_DIR)/$$(basename $$file .cpp).o; \
	    $(CXX) $(CXXFLAGS) $(NATIVE) -fprofile-instr-use=$(PROFDATA) -c $$file -o $$obj_file; \
	done
	@$(CXX) $(CXXFLAGS) $(NATIVE) -fprofile-instr-use=$(PROFDATA) $(OBJ_FILES) -o $(EXE)


# Separate release target without PGO
release: TARGET = Release
release: clean dirs $(OBJ_FILES) link

# Separate debug target
debug: TARGET = Debug
debug: clean dirs $(OBJ_FILES) link

dirs:
ifeq ($(OS),Windows_NT)
	@if not exist $(subst /,\,$(OBJ_DIR)) mkdir $(subst /,\,$(OBJ_DIR))
else
	@mkdir -p $(OBJ_DIR)
endif 

$(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) $(NATIVE) -c $< -o $@

link: $(OBJ_FILES)
	@echo "Linking"
	@$(CXX) $(CXXFLAGS) $(NATIVE) $(OBJ_FILES) -o $(EXE)

# Clean all build artifacts including object files and PGO data
clean:
	@echo "Cleaning object files, PGO data, and executable"
	@rm -rf $(OBJ_DIR) $(PROFILE_DIR) $(EXE)
