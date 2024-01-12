# Makefile

TOP_DIR = $(shell pwd)/..

include $(TOP_DIR)/Rules.mak
LIBNAME=$(TOP_DIR)/lib/dis.a

all : $(LIBNAME)

EXTRA_CFLAGS := -I../ds_public -I../snmp -I./common -I./db -I./cfg -I./sys -I./APP -I./ATS -I./SA -I./occ -I./TrainPosCall -I../gplat/gos_utility
EXTRA_LDFLAGS :=

CSRCS := ../snmp/snmp_nms.c

CPPSRCS := \
	common/DISPublic.cpp \
	cfg/TaskCfg.cpp \
	db/CfgDao.cpp \
	db/SyncDao.cpp \
	db/TaskDBSync.cpp \
	sys/ThreadGPS.cpp \
	sys/ThreadOMA.cpp \
	sys/TaskSys.cpp \
	SA/TaskSA.cpp \
	APP/TaskAPP.cpp \
	ATS/TaskATS.cpp \
	occ/TaskOCC.cpp \
	TrainPosCall/TaskTrainPosCall.cpp \
	TrainPosCall/ThreadTrainPosCall.cpp

clean-extra :
	- $(RM) $(LIBNAME)
	- $(RM) */*.o

include $(TOP_DIR)/Makerules
