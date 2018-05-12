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

CFLAGS += $(LIBDOS9_INC)
LDFLAGS += $(LIBDOS9_LD) $(PTHREAD_LD)

SRC_FILES := \
	src/Tea_Load.c src/Tea_Node.c src/Tea_Output.c \
	src/out/tea_ansi_out.c src/out/tea_html_out.c src/out/tea_text_out.c \
	src/tea.c src/tea_strupr.c

OBJ_FILES = $(SRC_FILES:.c=.o)

all: tea

tea: $(OBJ_FILES)
	$(CC) -o tea$(EXEC_SUFFIX) $(OBJ_FILES) $(LDFLAGS)

clean:
	rm -f $(OBJ_FILES)
	rm -f tea$(EXEC_SUFFIX)

bin: tea
	cp tea$(EXEC_SUFFIX) $(BINDIR)/cmd/

.PHONY: all clean bin
