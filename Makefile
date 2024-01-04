### Build Options ###

TARGET       := dukenukemzerohour
VERBOSE      ?= 0
BUILD_DIR    ?= build

ifeq ($(VERBOSE),0)
V := @
endif

ifeq ($(OS),Windows_NT)
  DETECTED_OS=windows
else
  UNAME_S := $(shell uname -s)
  ifeq ($(UNAME_S),Linux)
    DETECTED_OS=linux
  endif
  ifeq ($(UNAME_S),Darwin)
    DETECTED_OS=macos
    MAKE=gmake
    CPPFLAGS += -xc++
  endif
endif

### Output ###
DUKE_DIR     := DukeNukemZeroHour
SRC_DIR      := $(DUKE_DIR)/src
ASSETS_DIR   := assets
LIBULTRA_DIR := $(DUKE_DIR)/libs/libultra
LIBMUS_DIR   := $(DUKE_DIR)/libs/libmus
PC_DIR     	 := src
LD_MAP       := $(BUILD_DIR)/$(TARGET).map

### Tools ###

CROSS    :=
AS       := $(CROSS)as
LD       := $(CROSS)ld
OBJCOPY  := $(CROSS)objcopy
STRIP    := $(CROSS)strip
CPP      := $(CROSS)cpp
CC       := $(CROSS)gcc
CXX      := $(CROSS)g++

PRINT := printf '
 ENDCOLOR := \033[0m
 WHITE     := \033[0m
 ENDWHITE  := $(ENDCOLOR)
 GREEN     := \033[0;32m
 ENDGREEN  := $(ENDCOLOR)
 BLUE      := \033[0;34m
 ENDBLUE   := $(ENDCOLOR)
 YELLOW    := \033[0;33m
 ENDYELLOW := $(ENDCOLOR)
 PURPLE    := \033[0;35m
 ENDPURPLE := $(ENDCOLOR)
ENDLINE := \n'

### Compiler Options ###

OPTFLAGS       := -O0 -g
CFLAGS         := -m32 $(shell sdl2-config --cflags) -fsanitize=bounds-strict -fsanitize-undefined-trap-on-error -fstack-protector-all
CXXFLAGS       := -std=c++20
CPPFLAGS       := -I $(PC_DIR) -I $(DUKE_DIR)/include -I $(LIBULTRA_DIR)/include/2.0I -I $(LIBMUS_DIR)/include -D_LANGUAGE_C -DF3DEX_GBI_2 -D_FINALROM -DNON_MATCHING -DMODERN -DAVOID_UB
LDFLAGS        := undefined_syms.txt -Wl,-Map=$(LD_MAP) $(shell sdl2-config --libs) -static-libgcc -static-libstdc++
CHECK_WARNINGS := -Wall -Wextra -Wno-missing-braces -Wno-format-security -Wno-unused-parameter -Wno-unused-variable -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -Wno-unused-function

### Sources ###
SRC_DIRS := $(SRC_DIR)/code0 $(SRC_DIR)/code0/data $(SRC_DIR)/code1 $(SRC_DIR)/static \
            $(LIBULTRA_DIR)/src/audio $(LIBULTRA_DIR)/src/gu $(LIBMUS_DIR)/src $(PC_DIR) $(PC_DIR)/fast3d $(PC_DIR)/fast3d/glad $(PC_DIR)/audio $(PC_DIR)/controller
C_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
CXX_FILES := $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))

ASSETS_DIRS := $(ASSETS_DIR)/files
BIN_FILES := $(foreach dir,$(ASSETS_DIRS),$(wildcard $(dir)/*.bin))

# Object files
OBJECTS := $(foreach file,$(C_FILES),$(BUILD_DIR)/$(file:.c=.c.o))
OBJECTS += $(foreach file,$(CXX_FILES),$(BUILD_DIR)/$(file:.cpp=.cpp.o))
OBJECTS += $(foreach file,$(BIN_FILES),$(BUILD_DIR)/$(file:.bin=.bin.o))
OBJECTS := $(filter-out %.s.o, $(OBJECTS))

### Targets ###
$(BUILD_DIR)/$(LIBULTRA_DIR)/src/%.o: CPPFLAGS += -I $(LIBULTRA_DIR)/include/2.0I/PR
$(BUILD_DIR)/$(LIBMUS_DIR)/src/%.o: CPPFLAGS += -I $(LIBULTRA_DIR)/include/2.0I/PR -D_OLD_AUDIO_LIBRARY
$(BUILD_DIR)/$(PC_DIR)/%.o: CPPFLAGS += -DENABLE_OPENGL
$(BUILD_DIR)/$(PC_DIR)/assets.c.o: CPPFLAGS += -I $(LIBULTRA_DIR)/include/2.0I/PR -I $(LIBMUS_DIR)/src

all: $(BUILD_DIR)/$(TARGET)

CFLAGS += -MD -MP
-include $(OBJECTS:=.d)

clean:
	$(V)rm -rf $(BUILD_DIR)

# Compile .c files
$(BUILD_DIR)/%.c.o: %.c
	@$(PRINT)$(GREEN)Compiling C file: $(ENDGREEN)$(BLUE)$<$(ENDBLUE)$(ENDLINE)
	@mkdir -p $(shell dirname $@)
	$(V)$(CC) $(OPTFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

# Compile .c files
$(BUILD_DIR)/%.cpp.o: %.cpp
	@$(PRINT)$(GREEN)Compiling C++ file: $(ENDGREEN)$(BLUE)$<$(ENDBLUE)$(ENDLINE)
	@mkdir -p $(shell dirname $@)
	$(V)$(CXX) $(OPTFLAGS) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -c -o $@ $<

# Link the .o files
$(BUILD_DIR)/$(TARGET): $(OBJECTS)
	@$(PRINT)$(GREEN)Linking elf file: $(ENDGREEN)$(BLUE)$@$(ENDBLUE)$(ENDLINE)
	$(V)$(CXX) $^ $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) -o $@

# Create .o files from .bin files.
$(BUILD_DIR)/%.bin.o: %.bin
	@$(PRINT)$(GREEN)objcopying binary file: $(ENDGREEN)$(BLUE)$<$(ENDBLUE)$(ENDLINE)
	@mkdir -p $(shell dirname $@)
	$(V)$(LD) -r -b binary -o $@ $<

### Make Settings ###

.PHONY: all clean

# Remove built-in implicit rules to improve performance
MAKEFLAGS += --no-builtin-rules

# Print target for debugging
print-% : ; $(info $* is a $(flavor $*) variable set to [$($*)]) @true
