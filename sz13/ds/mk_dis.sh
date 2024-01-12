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

if [ ! -d "./bin/dis" ];then
    mkdir -p "./bin/dis"
fi

make -f Makefile_dis $@


