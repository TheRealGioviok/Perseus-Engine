TARGET ?= Release
OBJ_DIR = obj/$(TARGET)

CPP_FILES := $(wildcard *.cpp)
OBJ_FILES = $(addprefix $(OBJ_DIR)/, $(CPP_FILES:.cpp=.o))

CXX ?= g++
CXXFLAGS := -Wall -Wextra -Wpedantic -std=c++17 -Wno-implicit-fallthrough
NATIVE := -march=native -mpopcnt
EXE ?= Perseus

ifeq ($(TARGET), Release)
	CXXFLAGS += -funroll-loops -O3 -flto -fno-exceptions -DNDEBUG -lm
else ifeq ($(TARGET), Debug)
	CXXFLAGS += -g -O0 -fsanitize=address -fno-omit-frame-pointer -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC
else
	$(error "Unknown target: $(TARGET)")
endif

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
	$(CXX) $(CXXFLAGS) $(NATIVE) -c $< -o $@

link:
	@echo "Linking"
	$(CXX) $(CXXFLAGS) $(NATIVE) $(OBJ_FILES) -o $(EXE)

clean:
	@echo "Cleaning"
ifeq ($(OS),Windows_NT)
	@if exist $(subst /,\,$(OBJ_DIR)) rmdir /s /q $(subst /,\,$(OBJ_DIR))
	@if exist $(EXE).exe del $(EXE).exe
else
	@rm -rf $(OBJ_DIR)
	@rm -f $(EXE)
endif
