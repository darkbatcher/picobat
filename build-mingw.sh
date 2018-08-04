#!/bin/sh
CC=i686-w64-mingw32-gcc EXEC_SUFFIX=.exe CFLAGS="-O2 -s" LDFLAGS="-O2 -s" make $*
