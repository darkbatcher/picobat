#
#   pBat Makefiles, The pBat project
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
	./cmdlist/pBat_CmdList.c ./console/pBat_Console.c \
	./console/pBat_ConsoleANSI.c ./console/pBat_ConsoleWin.c \
	./console/pBat_ConsoleDummy.c ./dir/pBat_Attr.c ./dir/pBat_Dir.c \
	./dir/pBat_UnicodeDir.c ./enc/pBat_Enc.c ./estr/pBat_Estr.c \
	./stack/pBat_Stack.c ./thread/pBat_Thread.c ./file/pBat_StdFile.c \
	./file/pBat_Win32File.c ./init/pBat_Init.c ./cmdlib/pBat_String.c \
	./sleep/pBat_Sleep.c

CFLAGS += $(LIBCU8_INC) $(LIBPBAT_INC)

OBJ_FILES := $(SRC_FILES:.c=.o)

all: libpBat.a

libpBat.a: $(OBJ_FILES)
	$(AR) $@ $(OBJ_FILES)
	$(RANLIB) $@

clean:
	rm -f $(OBJ_FILES) libpBat.a

.PHONY: all tea clean
