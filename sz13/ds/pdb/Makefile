# Makefile

TOP_DIR = $(shell pwd)/..

include $(TOP_DIR)/Rules.mak
LIBNAME=$(TOP_DIR)/lib/pdb.a

all : $(LIBNAME)

EXTRA_CFLAGS := -Iinclude
EXTRA_LDFLAGS :=

CSRCS := \
	source/pdb_util.c

CPPSRCS := \
	source/pdb_app.cpp \
	source/pdb_dcci.cpp

ifdef HAVE_MYSQL
EXTRA_CFLAGS += -Imysql/include -Imysql/source

CSRCS += \
	mysql/source/pdb_mysql_core.c

CPPSRCS += \
	mysql/source/pdb_mysql_dcci.cpp
endif

ifdef HAVE_ORACLE
EXTRA_CFLAGS += -Ioracle/include

CSRCS += \
	oracle/source/pdb_oracle_core.c

CPPSRCS += \
	oracle/source/pdb_oracle_dcci.cpp
endif

clean-extra :
	- $(RM) $(LIBNAME)
	- $(RM) source/*.o mysql/source/*.o oracle/source/*.o

include $(TOP_DIR)/Makerules
