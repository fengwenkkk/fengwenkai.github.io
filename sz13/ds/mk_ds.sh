#!/bin/sh
#²ÎÊý£ºHAVE_MYSQL:=TRUE  PLAT_TYPE:=ARM VER:=V3.1.0

set -e

#export DODEBUG=1
export HAVE_MYSQL=TRUE

#export _VERSION=\"$(git_describe)\"

if [ ! -d "./lib" ];then
    mkdir -p "./lib"
fi

if [ ! -d "./bin" ];then
    mkdir -p "./bin"
fi

if [ ! -d "./bin/ds_chk" ];then
    mkdir -p "./bin/ds_chk"
fi

if [ ! -d "./bin/dis" ];then
    mkdir -p "./bin/dis"
fi

if [ ! -d "./bin/omc" ];then
    mkdir -p "./bin/omc"
fi

if [ ! -d "./bin/mrs" ];then
    mkdir -p "./bin/mrs"
fi

make -f Makefile_dis $@
make -f Makefile_dschk $@
make -f Makefile_tool $@

make -f Makefile_omc $@
make -f Makefile_mrs $@


