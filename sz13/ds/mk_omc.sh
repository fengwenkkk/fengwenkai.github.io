#!/bin/sh
#²ÎÊý£ºHAVE_MYSQL:=TRUE  PLAT_TYPE:=ARM PRODUCT_VERSION:=Vx.x.x

# set -e

# export DODEBUG=1
export HAVE_MYSQL=TRUE

if [ ! -d "./lib" ];then
    mkdir -p "./lib"
fi

if [ ! -d "./bin" ];then
    mkdir -p "./bin"
fi

if [ ! -d "./bin/omc" ];then
    mkdir -p "./bin/omc"
fi

make -f Makefile_omc -j20 $@
make -f Makefile_omc $@


