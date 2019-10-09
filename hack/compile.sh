#!/bin/bash

/usr/bin/c++  -DDMLC_USE_FOPEN64=0 -DTVM_LLVM_VERSION=40 -DUSE_MICRO_STANDALONE_RUNTIME=1 -I../include -I../3rdparty/dlpack/include -I../3rdparty/dmlc-core/include -I../3rdparty/rang/include -I../3rdparty/compiler-rt -I../3rdparty/picojson -I../vta/include -I/usr/lib/llvm-4.0/include -I../topi/include  -std=c++11 -faligned-new -O2 -Wall -fPIC     -DNDEBUG  -D_GNU_SOURCE  -D__STDC_CONSTANT_MACROS  -D__STDC_FORMAT_MACROS  -D__STDC_LIMIT_MACROS -pthread -o utvm.cc.o -c utvm.cc
/usr/bin/c++  -std=c++11 -faligned-new -O2 -Wall -fPIC   -rdynamic utvm.cc.o  -o utvm_test -Wl,-rpath,../build:/usr/lib/llvm-4.0/lib ../build/libtvm.so -Wl,-Bstatic -lgtest -Wl,-Bdynamic -lpthread -ldl -pthread /usr/lib/llvm-4.0/lib/libLLVM-4.0.so -ldl
