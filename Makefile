# Makefile

TOP_DIR = $(shell pwd)/../..

include $(TOP_DIR)/Rules.mak
LIBNAME=$(TOP_DIR)/lib/dms.a

all : $(LIBNAME)


EXTRA_CFLAGS := -I../../ds_public -I../../gos -I../../gplat
EXTRA_LDFLAGS :=

#CSRCS := ../../../ds_public/varm_cfg.c

CPPSRCS := \
	../../ds_public/cir.cpp \
	../../ds_public/ds_cfg.cpp \
	dms_event_msg.cpp \
	dms_file_msg.cpp \
	dms_public.cpp \
	dms_tran_msg.cpp
	

clean-extra :
	- $(RM) $(LIBNAME)
	- $(RM) */*.o

include $(TOP_DIR)/Makerules
