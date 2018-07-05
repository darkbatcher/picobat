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

SRC_FILES=dump.c

all: dump

dump$(EXEC_SUFFIX): $(SRC_FILES) dump.h
	$(CC) -o dump$(EXEC_SUFFIX) $(LDFLAGS) $(CFLAGS) $(SRC_FILES)

dump.h: dump_h_gen
	./dump_h_gen > dump.h

dump_h_gen$(EXEC_SUFFIX):	dump_h_gen.c
	$(CC) -o dump_h_gen $(LDFLAGS) $(CFLAGS) dump_h_gen.c

clean:
	rm -f dump$(EXEC_SUFFIX) dump_h_gen$(EXEC_SUFFIX)

bin: dump
	cp dump$(EXEC_SUFFIX) $(BINDIR)/cmd/

.PHONY: all clean bin
