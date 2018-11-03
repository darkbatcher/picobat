#
#   Dos9 Makefiles, The Dos9 project
#   Copyright (C) 2012-2018  Romain Garbi, Teddy Astie
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

ROOTDIR = ..
include ../config.mk

SRC_FILES := \
	./cmdlist/Dos9_CmdList.c ./console/Dos9_ConsoleANSI.c \
	./console/Dos9_ConsoleWin.c ./dir/Dos9_Attr.c ./dir/Dos9_Dir.c \
	./dir/Dos9_UnicodeDir.c \
	./enc/Dos9_Enc.c ./estr/Dos9_Estr.c stack/Dos9_Stack.c \
	./thread/Dos9_Thread.c ./file/Dos9_StdFile.c \
	./file/Dos9_Win32File.c ./init/Dos9_Init.c ./cmdlib/Dos9_String.c \
	./sleep/Dos9_Sleep.c

CFLAGS += $(LIBCU8_INC) $(LIBDOS9_INC)
	
OBJ_FILES := $(SRC_FILES:.c=.o)

all: libDos9.a

libDos9.a: $(OBJ_FILES)
	$(AR) $@ $(OBJ_FILES)
	$(RANLIB) $@

clean:
	rm -f $(OBJ_FILES) libDos9.a

.PHONY: all tea clean
