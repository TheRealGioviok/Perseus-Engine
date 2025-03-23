TARGET ?= Release
OBJ_DIR = obj/$(TARGET)

CPP_FILES := $(wildcard *.cpp)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(CPP_FILES:.cpp=.o))

ifeq ($(OS),Windows_NT)
ifeq ($(CXX),clang++)
# disable msvc linker (beacuse bad)
	LINKER := -fuse-ld=lld-link
endif
endif

CXX ?= clang++
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++20 -Wno-implicit-fallthrough
EXE ?= Perseus
CPU ?= default

ifeq ($(TARGET), Release)
#	-lm in g++ means to link the math library; in clang++ it means to link the library named 'm.lib' (which doesn't exist).
#	since the math library *should* be linked by default it's being removed from the flags. in case of errors, add an if that adds it in case of g++.
	CXXFLAGS += -funroll-loops -O3 -flto -fno-exceptions -DNDEBUG
else ifeq ($(TARGET), Debug)
	CXXFLAGS += -g -O0 -fsanitize=address -fno-omit-frame-pointer -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
else
	$(error "Unknown target: $(TARGET)")
endif

ifeq ($(CPU), default)
    CPU_FLAGS := -march=native
else ifeq ($(CPU), M64)
    CPU_FLAGS := -m64 -mpopcnt
else ifeq ($(CPU), haswell)
    CPU_FLAGS := -march=haswell -m64
else ifeq ($(CPU), znver2)
    CPU_FLAGS := -march=znver2
else ifeq ($(CPU), native)
    CPU_FLAGS := -march=native
else
    $(error "Unknown CPU architecture: $(CPU)")
endif

CXXFLAGS += $(CPU_FLAGS)

.PHONY: all build dirs link clean

all: build

build: dirs $(OBJ_FILES) link 

dirs:
ifeq ($(OS),Windows_NT)
	@if not exist $(subst /,\,$(OBJ_DIR)) mkdir $(subst /,\,$(OBJ_DIR))
else
	@mkdir -p $(OBJ_DIR)
endif 

$(OBJ_DIR)/%.o: %.cpp
	@echo "Compiling $<"
	@$(CXX) $(CXXFLAGS) -c $< -o $@

link: $(OBJ_FILES)
	@echo "Linking"
	@$(CXX) $(CXXFLAGS) $(LINKER) $(OBJ_FILES) -o $(EXE)

clean:
	@echo "Cleaning"
ifeq ($(OS),Windows_NT)
	@if exist $(subst /,\,$(OBJ_DIR)) rmdir /s /q $(subst /,\,$(OBJ_DIR))
	@if exist $(EXE) del $(EXE)
else
	@rm -rf $(OBJ_DIR)
	@rm -f $(EXE)
endif
