# Makefile

TOP_DIR = $(shell pwd)/../..

include $(TOP_DIR)/Rules.mak

TOOL_DIR = $(TOP_DIR)/tool
SRC_DIR = $(TOOL_DIR)/procmon
EXE=$(TOOL_DIR)/bin/procmon

all: $(EXE)

INC_FLAG = $(CC_PUB_INC) \
		-I $(SRC_DIR) \
		-I $(TOP_DIR)/gos/public

EXTRA_CFLAGS := $(INC_FLAG)
#-DHAVE_CONFIG_H -D_OSLINUX_
EXTRA_LDFLAGS :=

DIRS =
#$(TOP_DIR)/plat gos

LIBS += $(TOP_DIR)/lib/gos.a \
		$(SRC_DIR)/procmon.o \
		$(SRC_DIR)/procfunc.o

SRCS := ./procmon.cpp \
		./procfunc.cpp

CPPOBJS=$(patsubst %.cpp,%.o,$(SRCS))

$(EXE) : $(COBJS) $(CPPOBJS) $(LIBS)
	$(GCC) $(INC_FLAG) $(LDFLAGS) $(CPPOBJS) -Xlinker "-(" $(LIBS) -Xlinker "-)" -o $@

ifeq ($(DODEBUG),)
	$(STRIPTOOL) $(STRIP_FLAGS) $@
endif

clean-extra :
	- $(RM) $(EXE) $(EXE).map
	- $(RM) *.o $(LIBALL)

include $(TOP_DIR)/Makerules
