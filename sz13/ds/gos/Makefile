# Makefile

TOP_DIR = $(shell pwd)/..

include $(TOP_DIR)/Rules.mak

LIBNAME=$(TOP_DIR)/lib/gos.a

export TOP_DIR

all : $(LIBNAME)

EXTRA_CFLAGS := -I. -Ipublic -Ixml
EXTRA_LDFLAGS :=

CSRCS := \
	func/gos_api.c \
	func/gos_conf_file.c \
	func/gos_file.c \
	func/gos_func.c \
	func/gos_md5.c \
	func/gos_unicode.c \
	comm/gos_ping.c \
	mem/gos_mem.c \
	queue/gos_hash.c \
	queue/gos_list.c \
	queue/gos_hashlist.c \
	xml/gos_xml_api.c \
	xml/gos_xml_parse.c \
	zip/quicklz/quicklz.c \
	zip/quicklz/gos_quicklz.c
	
CPPSRCS := \
	func/g_conf_file.cpp \
	func/g_filedb.cpp \
	func/g_json.cpp \
	func/g_lock.cpp \
	func/g_logger.cpp \
	func/g_string.cpp \
	func/g_thread.cpp \
	func/g_proc.cpp \
	comm/g_telnet.cpp \
	mem/g_mem.cpp \
	queue/g_hash.cpp \
	queue/g_hashlist.cpp \
	queue/g_list.cpp \
	xml/g_xml.cpp \
	func/g_log.cpp \
	util/g_string_util.cpp

clean-extra :
	- $(RM) $(LIBNAME)
	- $(RM) */*.o
	- $(RM) */quicklz/*.o

include $(TOP_DIR)/Makerules
