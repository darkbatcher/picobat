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
ifneq (,$(wildcard ./femto-config.mk))
    include femto-config.mk
endif
include ../config.mk

ifdef lib_m
LDFLAGS += -lm
endif

SRC_FILES = lib/fasteval.c lib/parser.c lib/scanner.c \
	lib/xmath.c

OBJ_FILES = $(SRC_FILES:.c=.o)

all: libfasteval.a

clean:
	rm -f $(OBJ_FILES) lib/scanner.c lib/parser.c lib/parser.h libfasteval.a

bin:

libfasteval.a: $(OBJ_FILES)
	$(AR) r $@ $(OBJ_FILES)
	$(RANLIB) $@

lib/scanner.c: lib/scanner.l lib/parser.c
	$(LEX) -Pevaluator_ -L -o$@ lib/scanner.l

lib/parser.c: lib/parser.y
	$(YACC) -v -g -d -p evaluator_ -o $@ $^
	
# stuff to check
PROGRAMS =
FUNCTIONS =
LIBS = m
OPTIONS =
DEFAULTOPTIONS = 
SUBCONFIG = 
ADDITIONALVARS = 

include ../femto.mk

.PHONY: all clean bin
