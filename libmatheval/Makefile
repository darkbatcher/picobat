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

CFLAGS += -DHAVE_STDINT_H -DHAVE_CONFIG_H -DSTDC_HEADERS

SRC_FILES = \
	lib/error.c lib/matheval.c lib/node.c lib/parser.c lib/scanner.c \
	lib/symbol_table.c lib/xmalloc.c lib/xmath.c

OBJ_FILES = $(SRC_FILES:.c=.o)

all: libmatheval.a

clean:
	rm -f $(OBJ_FILES) lib/scanner.c lib/parser.c lib/parser.h libmatheval.a

bin:

libmatheval.a: $(OBJ_FILES)
	$(AR) $@ $(OBJ_FILES)
	$(RANLIB) $@

lib/scanner.c: lib/scanner.l lib/parser.c
	$(LEX) -Pevaluator_ -L -o$@ lib/scanner.l

lib/parser.c: lib/parser.y
	$(YACC) -d -p evaluator_ -o $@ $^

.PHONY: all clean bin
