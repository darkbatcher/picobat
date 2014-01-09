# Dos9, The Dos9 Project - Copyright © 2010-2014 Darkbatcher
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Please report bugs, and mistakes at : <darkbatcher at dos9 dot org>

SRC=./lib/inteval.c ./lib/parser.c ./lib/lexer.c 
	
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
INCLUDE_FILE=$(INC_DIR)/inteval.h
ORIGIN_FILE=./lib/inteval.h

# build filenames
TARGET=$(LIB_DIR)/libinteval.a
OBJ=$(SRC:.c=.o)

all : $(TARGET) $(INCLUDE_FILE)

# construction de la librarie
$(TARGET): $(OBJ)
			$(AR) $(AFLAGS) $@ $^

# copie du fichier matheval.h original vers le dossier ``./include''
$(INCLUDE_FILE): $(ORIGIN_FILE)
	cp $(ORIGIN_FILE) $(INCLUDE_FILE)
			
#compilation des fichiers C
%.o : %.c
			$(CC) -o $@ -c $< $(CFLAGS) -D HAVE_CONFIG_H

# constuction des fichiers YACC			
%.c : %.y
			$(YACC) -d $< -p IntEval_ -o $@

# construction des fichiers LEX
%.c : %.l
			$(LEX) -o $@ -P IntEval_ $<

			