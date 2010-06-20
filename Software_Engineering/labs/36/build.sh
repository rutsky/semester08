#!/bin/bash

export MOZSDKDIR=/usr/lib/xulrunner-devel-1.9.1.9
#export MOZSDKDIR=/home/bob/stuff/firefox/xulrunner-sdk # 32bit SDK :(
export TOPSRCDIR=`pwd`
export XULRUNNER_BIN=/usr/bin/xulrunner
#make crypto rebuild_crypto=1
#make crypto
#make clean
#make xpi
#make build

#make -C crypto build
#make crypto-obsolete-build
#make clean && make buildall && make onlyxpi
make clean && make buildall
