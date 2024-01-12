# Makefile

TOP_DIR = $(shell pwd)/..

include $(TOP_DIR)/Rules.mak
LIBNAME=$(TOP_DIR)/lib/gplat.a

all : $(LIBNAME)

EXTRA_CFLAGS := -I.
EXTRA_LDFLAGS :=

CSRCS :=

CPPSRCS := \
	GBaseTask.cpp \
	GCommon.cpp \
	GMsgDispatch.cpp \
	GMsgPool.cpp \
	GMsgQueue.cpp \
	GProc.cpp \
	GSysTask.cpp \
	GTimer.cpp \
	GSysLogTask.cpp \
	GTransFile.cpp \
	GTransFileClient.cpp

clean-extra :
	- $(RM) $(LIBNAME)
	- $(RM) */*.o

include $(TOP_DIR)/Makerules
