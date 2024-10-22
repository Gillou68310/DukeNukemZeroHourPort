# detect operation system
UNAME ?= $(shell uname -s)
OS := NONE
ifeq ("$(UNAME)","Linux")
  OS = LINUX
endif
ifeq ("$(UNAME)","linux")
  OS = LINUX
endif
ifneq ("$(filter GNU hurd,$(UNAME))","")
  OS = LINUX
endif
ifeq ("$(UNAME)","Darwin")
  OS = OSX
endif
ifeq ("$(UNAME)","FreeBSD")
  OS = FREEBSD
endif
ifeq ("$(UNAME)","OpenBSD")
  OS = FREEBSD
endif
ifneq ("$(filter GNU/kFreeBSD kfreebsd,$(UNAME))","")
  OS = LINUX
endif
ifeq ("$(patsubst MINGW%,MINGW,$(UNAME))","MINGW")
  OS = MINGW
endif
ifeq ("$(OS)","NONE")
  $(error OS type "$(UNAME)" not supported.')
endif

# detect system architecture, only if it matters for build flags
HOST_CPU ?= $(shell uname -m)
CPU := OTHER
ifneq ("$(filter x86_64 amd64,$(HOST_CPU))","")
  CPU := X86
  ifeq ("$(BITS)", "32")
    ARCH_DETECTED := 64BITS_32
  else
    ARCH_DETECTED := 64BITS
  endif
endif
ifneq ("$(filter pentium i%86,$(HOST_CPU))","")
  CPU := X86
  ARCH_DETECTED := 32BITS
endif

VERSION ?= us
ROOTDIR = ../..
DUKEDIR = $(ROOTDIR)/DukeNukemZeroHour
LIBSDIR = $(DUKEDIR)/libs
SRCDIR = $(ROOTDIR)/src
LIBULTRADIR = $(LIBSDIR)/libultra
LIBMUSDIR = $(LIBSDIR)/libmus
OBJDIR = _obj

# base CFLAGS, LDLIBS, and LDFLAGS
OPTFLAGS ?= -O2
WARNFLAGS ?= -Wall -Wno-missing-braces -Wno-unused-variable -Wno-unused-function -Wno-unused-result -Wno-implicit-function-declaration -Wno-incompatible-pointer-types -Wno-int-conversion
CFLAGS += $(WARNFLAGS) $(OPTFLAGS)
CXXFLAGS += -std=c++20

CPPFLAGS := -I $(SRCDIR) -I $(DUKEDIR)/include -I $(LIBULTRADIR)/include/2.0I -I $(LIBMUSDIR)/include \
            -I $(LIBULTRADIR)/include/2.0I/PR -I $(LIBMUSDIR)/src -D_LANGUAGE_C -DF3DEX_GBI_2 \
            -D_FINALROM -DNON_MATCHING -DMODERN -DAVOID_UB -D_OLD_AUDIO_LIBRARY -DENABLE_OPENGL

LDLIBS += -static-libgcc -static-libstdc++

ifeq ($(OS), LINUX)
  LDLIBS += -ldl
endif

ifeq ($(DEBUG), 1)
CFLAGS += -g
OPTFLAGS =
#CFLAGS += -fsanitize=bounds-strict -fsanitize-undefined-trap-on-error -fstack-protector-all
endif

ifeq ($(VERSION),us)
    CPPFLAGS += -DVERSION_US=1
else ifeq ($(VERSION),fr)
    CPPFLAGS += -DVERSION_FR=1
else ifeq ($(VERSION),eu)
    CPPFLAGS += -DVERSION_EU=1
else
$(error Invalid VERSION variable detected. Please use either 'us' or 'fr' or 'eu')
endif

# tweak flags for 32-bit build on 64-bit system
ifeq ($(ARCH_DETECTED), 64BITS_32)
  CFLAGS += -m32
endif

# test for essential build dependencies
ifeq ($(origin PKG_CONFIG), undefined)
  PKG_CONFIG = $(CROSS_COMPILE)pkg-config
  ifeq ($(shell which $(PKG_CONFIG) 2>/dev/null),)
    $(error $(PKG_CONFIG) not found)
  endif
endif

# test for presence of SDL
ifeq ($(origin SDL_CFLAGS) $(origin SDL_LDLIBS), undefined undefined)
  SDL_CONFIG = $(CROSS_COMPILE)sdl2-config
  ifeq ($(shell which $(SDL_CONFIG) 2>/dev/null),)
    $(error No SDL2 development libraries found!)
  endif
  SDL_CFLAGS += $(shell $(SDL_CONFIG) --cflags)
  SDL_LDLIBS += $(shell $(SDL_CONFIG) --libs)
endif
CFLAGS += $(SDL_CFLAGS)
LDLIBS += $(SDL_LDLIBS)

# reduced compile output when running make without V=1
ifneq ($(findstring $(MAKEFLAGS),s),s)
ifndef V
	Q_CC  = @echo '    CC  '$@;
	Q_CXX = @echo '    CXX '$@;
	Q_LD  = @echo '    LD  '$@;
endif
endif

# set base program pointers and flags
CC    = $(CROSS_COMPILE)gcc
CXX   = $(CROSS_COMPILE)g++
RM    ?= rm -f
MKDIR ?= mkdir -p
COMPILE.c = $(Q_CC)$(CC) $(CFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
COMPILE.cc = $(Q_CXX)$(CXX) $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(TARGET_ARCH) -c
LINK.o = $(Q_LD)$(CXX) $(CFLAGS) $(CXXFLAGS) $(LDFLAGS) $(TARGET_ARCH)

ifndef PIE
  ifeq ($(CC),$(CROSS_COMPILE)gcc)
    # check if PIE is the default for the compiler
    PIE_DEFAULT = $(shell $(CC) -v 2>&1 | grep enable-default-pie)
    ifeq ($(PIE_DEFAULT),)
      PIE = 1
    endif
  endif
endif

ifdef PIE
  ifeq ($(PIE), 1)
    CFLAGS += -fPIE
    LDFLAGS += -pie
  else
    ifeq ($(PIE), 0)
      CFLAGS += -fno-PIE
      LDFLAGS += -no-pie
    else
      $(error Invalid PIE value.)
    endif
  endif
endif

ifeq ("$(OS)","MINGW")
  EXEEXT = .exe
else
  EXEEXT =
endif

# list of source files to compile
SRCDIRS := $(DUKEDIR)/src/code0 $(DUKEDIR)/src/code0/data $(DUKEDIR)/src/code0/data/actors $(DUKEDIR)/src/code1 $(DUKEDIR)/src/static \
           $(LIBULTRADIR)/src/audio $(LIBULTRADIR)/src/gu $(LIBMUSDIR)/src \
           $(SRCDIR) $(SRCDIR)/fast3d $(SRCDIR)/fast3d/glad $(SRCDIR)/audio $(SRCDIR)/controller

SOURCE = $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.c))
SOURCE += $(foreach dir,$(SRCDIRS),$(wildcard $(dir)/*.cpp))

# generate a list of object files build, make a temporary directory for them
OBJECTS := $(patsubst $(ROOTDIR)/%.c, $(OBJDIR)/%.o, $(filter %.c, $(SOURCE)))
OBJECTS += $(patsubst $(ROOTDIR)/%.cpp, $(OBJDIR)/%.o, $(filter %.cpp, $(SOURCE)))
OBJDIRS = $(dir $(OBJECTS))
$(shell $(MKDIR) $(OBJDIRS))

# build targets
TARGET = dukenukemzerohour$(EXEEXT)

targets:
	@echo "Duke Nukem Zero Hour makefile."
	@echo "  Targets:"
	@echo "    all            == Build Duke Nukem Zero Hour application"
	@echo "    clean          == remove object files and build products"
	@echo "  Options:"
	@echo "    BITS=32        == build 32-bit binaries on 64-bit machine"
	@echo "    OPTFLAGS=flags == compiler optimization (default: -O2)"
	@echo "    WARNFLAGS=flag == compiler warning levels (default: -Wall)"
	@echo "    PIE=(1|0)      == Force enable/disable of position independent executables"
	@echo "  Debugging Options:"
	@echo "    DEBUG=1        == add debugging symbols to application binary"
	@echo "    V=1            == show verbose compiler output"

all: $(TARGET)

clean:
	$(RM) -r $(OBJDIR) $(TARGET)

# build dependency files
CFLAGS += -MD -MP
-include $(OBJECTS:.o=.d)

# standard build rules
$(OBJDIR)/%.o: $(ROOTDIR)/%.c
	$(COMPILE.c) -o $@ $<

$(OBJDIR)/%.o: $(ROOTDIR)/%.cpp
	$(COMPILE.cc) -o $@ $<

$(TARGET): $(OBJECTS)
	$(LINK.o) $^ $(LDLIBS) -o $@

.PHONY: all clean
