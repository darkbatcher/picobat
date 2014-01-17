# Copyright (C) 1999, 2002, 2003, 2004, 2005, 2006, 2007, 2008, 2011,
# 2012, 2013 Free Software Foundation, Inc.
#
# Copyright (C) 2013, 2014 Romain Garbi (DarkBatcher)
# 
# This file is part of GNU libmatheval
# 
#  GNU libmatheval is free software: you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or (at your option) any later version.
#
# GNU libmatheval is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU libmatheval.  If not, see
# <http://www.gnu.org/licenses/>.
#

SRC=./lib/error.c ./lib/matheval.c ./lib/node.c \
	./lib/parser.c ./lib/scanner.c ./lib/symbol_table.c ./lib/xmalloc.c \
	./lib/xmath.c
	
# flags for LEX/YACC
YACC=bison
LEX=flex

# flags for c compiler
CFLAGS=-Wall -O3 -g

# flags for lib compiler
AFLAGS=-r -s

# the compilers vars
CC=gcc
AR=ar

# setup includes
INCLUDE_FILE=$(INC_DIR)/matheval.h
ORIGIN_FILE=./lib/matheval.h

# build filenames
TARGET=$(LIB_DIR)/libmatheval.a
OBJ=$(SRC:.c=.o)

all : $(TARGET) $(INCLUDE_FILE)

# build the lib
$(TARGET): $(OBJ)
			$(AR) $(AFLAGS) $@ $^

# installation of header
$(INCLUDE_FILE): $(ORIGIN_FILE)
	cp $(ORIGIN_FILE) $(INCLUDE_FILE)
			
# build C files
%.o : %.c
			$(CC) -o $@ -c $< $(CFLAGS) -D HAVE_CONFIG_H

# build YACC files			
%.c : %.y
			$(YACC) -d $< -o $@

# build LEX files
%.c : %.l
			$(LEX) -o $@ $<