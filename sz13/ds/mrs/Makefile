# Makefile

TOP_DIR = $(shell pwd)/..

include $(TOP_DIR)/Rules.mak
LIBNAME=$(TOP_DIR)/lib/mrs.a

all : $(LIBNAME)


#EXTRA_CFLAGS := -I../ds_public -I../ds_public -I../ffmpeg_api

EXTRA_LDFLAGS :=

LDFLAGS += -Wno-error=deprecated-declarations
EXTRA_LDFLAGS += -Wno-error=deprecated-declarations

#TD_SDK_HOME:=/home/leijie/TD_SDK
TD_SDK_HOME:=${TOP_DIR}/../TD_SDK

TD_SDK_INC:=${TD_SDK_HOME}/include

CSRCS := ../snmp/snmp_nms.c

#	../ffmpeg_api/ffmpeg_api.cpp \

CPPSRCS := \
	CfgDao.cpp \
	rec_util.cpp \
	connectionControl.cpp \
	RECDisplayMnager.cpp \
	RecPublic.cpp \
	TaskCfg.cpp \
	TaskCheck.cpp \
	TaskSys.cpp \
	ThreadOMA.cpp

clean-extra :
	- $(RM) $(LIBNAME)
	- $(RM) */*.o

include $(TOP_DIR)/Makerules
