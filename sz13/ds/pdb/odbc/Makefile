# 定义运行的目标环境：1） HOST 表示运行在pc的虚拟机上； 2） IXP425 表示运行在盒子上。
RUNTIME_CPU := HOST

# 定义工程类型、输出文件和obj目录。
OUTPUT_FILE := libpdb.a
OBJ_DIR = ../obj
LIB_DIR = ../../lib
INSTALL = /usr/bin/install -c

# 增加各自模块的include路径和source路径，注意每个source都需要以:结尾。
ROOT := .

INCLUDE_PATH := -I$(ROOT) \
	            -I$(ROOT)/oracle \
	            -I$(ROOT)/oracle/linux

SOURCES_PATH := ./:

ifeq ($(RUNTIME_CPU),IXP425)
CC := /mnt/disk/ixp425/buildroot/build_armeb/staging_dir/bin/armeb-linux-gcc
endif
ifeq ($(RUNTIME_CPU),HOST)
CC := gcc
endif

LINK=$(CC)

CFLAGS=$(INCLUDE_PATH) -Wall -Wstrict-prototypes
CFLAGS += -DOS_LINUX
CFLAGS += -c -g

LDFLAGS += -Wl -lpthread -lrt

vpath %.c $(SOURCES_PATH)
SOURCES = $(wildcard $(subst :,/*.c,$(SOURCES_PATH)))
SOURCES_NODIR = $(notdir $(SOURCES))
OBJS = $(patsubst %.c, $(OBJ_DIR)/%.o, $(SOURCES_NODIR))

TARGETS = $(OBJ_DIR)/$(OUTPUT_FILE)

$(OUTPUT_FILE): $(OBJS)
	$(AR) rcs $@ $(OBJS)
	cp $@ $(LIB_DIR)

all : $(OBJ_DIR) $(TARGETS) $(OBJ_DIR)

$(TARGETS) : $(OBJS) $(OBJ_DIR)
	$(LINK) -o $@ $(OBJS) $(LFLAGS)

$(OBJ_DIR) :
	mkdir $@

$(OBJ_DIR)/%.o : %.c
	$(LINK) $(CFLAGS) -o $@ $<

.PHONY : clean install

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/$(OUTPUT_FILE) $(LIB_DIR)/$(OUTPUT_FILE)

install:
	$(INSTALL) -m 755 $(OBJ_DIR)/$(OUTPUT_FILE) $(OUTPUT_FILE)

