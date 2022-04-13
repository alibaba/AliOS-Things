#!/bin/sh

TARGET=x86_64-w64-mingw32
INSTALL_PATH=/usr/local/$TARGET
export PATH=$PATH:/usr/local/bin

make -f win32/Makefile.gcc \
    SHARED_MODE=1 \
    LDFLAGS=-static-libgcc \
    PREFIX=$TARGET- \
    prefix=$INSTALL_PATH \
    BINARY_PATH=$INSTALL_PATH/bin \
    INCLUDE_PATH=$INSTALL_PATH/include \
    LIBRARY_PATH=$INSTALL_PATH/lib $*
