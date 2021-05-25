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

CFLAGS += $(LIBPBAT_INC)
LDFLAGS += $(LIBPBAT_LD)
SRC_FILES = pbatize.c pairs.c

all: pbatize

pbatize:
	$(CC) -o pbatize$(EXEC_SUFFIX) $(SRC_FILES) $(LDFLAGS) $(CFLAGS)

clean:
	rm -f pbatize$(EXEC_SUFFIX)

bin: pbatize
	cp pbatize$(EXEC_SUFFIX) $(BINDIR)/cmd/

.PHONY: all clean bin
