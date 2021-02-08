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
include $(ROOTDIR)/config.mk

ifneq (,$(wildcard ./femto-config.mk))
    include femto-config.mk
endif

CFLAGS += $(LIBCU8_INC) -I./include

SRC_FILES = src/init.c src/inject.c src/encoding.c src/read.c src/write.c \
			src/fdio.c src/spawn.c src/stat.c src/dup.c src/find.c src/dir.c \
			src/dirent.c src/iconv/iconv.c

OBJ_FILES = $(SRC_FILES:.c=.o)

all: libcu8.a

clean:
	rm -f $(OBJ_FILES) libcu8.a
	$(MAKE) -C test clean

test: 	
	$(MAKE) -C test run-tests
	
libcu8.a: $(OBJ_FILES)
	$(AR) $@ $(OBJ_FILES)
	$(RANLIB) $@	
	
bin:

# stuff to check
PROGRAMS =
LIBS = iconv
FUNCTIONS = _findfirst _findfirst32 _findfirst64 _findfirst32i64 \
			_findfirst64i32 _stat _stat32 _stat64 _stat64i32 \
			_stat32i64
OPTIONS = 
DEFAULTOPTIONS = 
SUBCONFIG = 
ADDITIONALVARS = 

include ../femto.mk

.PHONY: all bin clean $(SUBDIRS) $(SUBDIRS_CLEAN) po-merge test