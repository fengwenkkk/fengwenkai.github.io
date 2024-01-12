# Rules.make

# check for proper make version
ifneq ($(findstring x3.7,x$(MAKE_VERSION)),)
$(error Your make is too old $(MAKE_VERSION). Go get at least 3.80)
endif

# If you are running a cross compiler, you will want to set
# 'CROSS' to something more interesting ...  Target
# architecture is determined by asking the CC compiler what
# arch it compiles things for, so unless your compiler is
# broken, you should not need to specify TARGET_ARCH.
#
# Most people will set this stuff on the command line, i.e.
#        make CROSS=arm-linux-
# will build for 'arm'.

ifeq ($(shell getconf LONG_BIT), 64)
    ARCH_64 = TRUE
else
    ARCH_32 = TRUE
endif

ifndef CROSS
    CROSS =
endif

ifeq ($(PLAT_TYPE), ARM)
    CROSS=mips-openwrt-linux-uclibc-
    ifndef TTY_NAME
        TTY_NAME = 0
    endif
endif

CC         = $(CROSS)gcc
GCC        = $(CROSS)g++
AR         = $(CROSS)ar
LD         = $(CROSS)ld
NM         = $(CROSS)nm
OBJDUMP    = $(CROSS)objdump
STRIPTOOL  = $(CROSS)strip
DEPEND     = $(CC) -MM

INSTALL    = install
LN         = ln
RM         = rm -f
TAR        = tar
SED        = sed
AWK        = awk

STRIP_FLAGS ?= -x -R .note -R .comment

# Select the compiler needed to build binaries for your development system
HOSTCC     = gcc

#BUILD_CFLAGS = -Os -Wall
BUILD_CFLAGS = -Os -Wall -Wno-parentheses -fno-strict-aliasing

# strip quotes
qstrip = $(strip $(subst ",,$(1)))
#"))

# Ensure consistent sort order, 'gcc -print-search-dirs' behavior, etc.
LC_ALL := C
export LC_ALL

ARFLAGS:=crs

ASFLAGS:=

# Flags in OPTIMIZATION are used only for non-debug builds

OPTIMIZATION:=-Os

CPU_CFLAGS := -fsigned-char -fno-builtin

ifneq ($(PLAT_TYPE), ARM)
#CPU_CFLAGS += -march=i686
endif

PICFLAG :=
#WARN_CFLAGS := -Wformat -Wimplicit-function-declaration -Wreturn-type -Wchar-subscripts -Werror
#WARN_CFLAGS := -Wall -Werror
WARN_CFLAGS := -Wformat
WARN_CFLAGS += -Wreturn-type
WARN_CFLAGS += -Wno-error=unused-parameter
WARN_CFLAGS += -Wno-error=unused-variable
WARN_CFLAGS += -Wno-error=unused-value
WARN_CFLAGS += -Wuninitialized
WARN_CFLAGS += -Werror=write-strings
WARN_CFLAGS += -Werror=deprecated-declarations
LARGEFILE_CFLAGS :=-D_LARGEFILE_SOURCE -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

PLAT_FLAGS :=

#CFLAGS += -O2 -Wall -fomit-frame-pointer -g -ansi

ifeq ($(PLAT_TYPE), ARM)
WARN_CFLAGS += -Wno-error=uninitialized -Wno-error=literal-suffix
WARN_CFLAGS += -Wno-error=implicit-fallthrough
WARN_CFLAGS += -Wno-error=write-strings
endif

CFLAGS :=-pipe $(CPU_CFLAGS) $(PICFLAG) $(WARN_CFLAGS) $(LARGEFILE_CFLAGS) $(PLAT_FLAGS)
CPPFLAGS:=-pipe $(CPU_CFLAGS) $(PICFLAG) $(WARN_CFLAGS) $(LARGEFILE_CFLAGS) $(PLAT_FLAGS) 
CPPFLAGS += -Wno-literal-suffix

#CPPFLAGS += -O2 -Wall -g

LIBS :=
#LDFLAGS = -lpthread -lrt -ldl -lcrypt -lz -L$(TOP_DIR)/lib
LDFLAGS = -lpthread -lrt -ldl -lcrypt -L$(TOP_DIR)/lib

HAVE_SHARED:=$(call qstrip,$(HAVE_SHARED))
ifeq ($(HAVE_SHARED),)
    CFLAGS +=
endif

DODEBUG:=$(call qstrip,$(DODEBUG))
ifneq ($(DODEBUG),)
    CFLAGS += -O0 -g3 -DDEBUG -DGOS_DEBUG
    CPPFLAGS += -O0 -g3 -DDEBUG -DGOS_DEBUG
else
    CFLAGS += $(OPTIMIZATION) -DG_DISABLE_ASSERT -DNDEBUG -O3
    CPPFLAGS += $(OPTIMIZATION) -DG_DISABLE_ASSERT -DNDEBUG -O3
    LDFLAGS += -Wl,-Map,$@.map
endif

ifneq ($(DODEBUG),)
    git_describe=$(shell git rev-parse HEAD)
    CPPFLAGS += -DGIT_LATEST_COMMIT_ID=\"$(git_describe)\"
endif

ifdef TTY_NAME
    CFLAGS += -DTTY_NAME=$(TTY_NAME)
    CPPFLAGS += -DTTY_NAME=$(TTY_NAME)
endif

ifdef ARCH_64
    CFLAGS += -DARCH_64
    CPPFLAGS += -DARCH_64
endif

CFLAGS += -DGOS_USE_SEM -DGOS_USE_PTHREAD
CPPFLAGS += -DGOS_USE_SEM -DGOS_USE_PTHREAD

#LDFLAGS += -Wl,--stack,0x1000000

CC_IPREFIX := $(shell $(CC) --print-file-name=include)
CC_INC := -isystem $(CC_IPREFIX)
CC_PUB_INC := -I$(TOP_DIR)/gos/public \
              -I$(TOP_DIR)/pdb/include \
              -I$(TOP_DIR)/dtp \
              -I$(TOP_DIR)/gplat \
              -I./

CFLAGS += $(CC_INC) $(CC_PUB_INC)
CPPFLAGS += $(CC_INC) $(CC_PUB_INC)

CFLAGS_gen.dep = -MT $@ -MD -MP -MF $(dir $@).$(notdir $@).dep
