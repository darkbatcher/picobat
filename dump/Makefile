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

SRC_FILES=dump.c

all: dump$(EXEC_SUFFIX)

dump_h_gen$(EXEC_SUFFIX):	dump_h_gen.c
	$(CC) -o dump_h_gen$(EXEC_SUFFIX) $(LDFLAGS) $(CFLAGS) dump_h_gen.c

dump.h: dump_h_gen
	./dump_h_gen$(EXEC_SUFFIX) > dump.h

dump$(EXEC_SUFFIX): dump.h $(SRC_FILES)
	$(CC) -o dump$(EXEC_SUFFIX) $(LDFLAGS) $(CFLAGS) $(SRC_FILES)

clean:
	rm -f dump$(EXEC_SUFFIX) dump_h_gen$(EXEC_SUFFIX) dump.h

bin: dump
	cp dump$(EXEC_SUFFIX) $(BINDIR)/cmd/

.PHONY: all clean bin
