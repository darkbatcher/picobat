# libDos9, The Dos9 Project - Copyright © 2010-2013 Darkbatcher
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Please report bugs, and mistakes at : <darkbatcher at dos9 dot org>
#

# flags pour  c compiler
CFLAGS=-Wall -O3 -g

# flags for lib compiler
AFLAGS=-r -s

# the compilers vars
CC=gcc
AR=ar

TARGET=../$(LIB_DIR)/libDos9.a

INCLUDE_FILE=../$(INC_DIR)/libDos9.h
ORIGIN_FILE=./libDos9.h

#the list of the sources
SRC=./cmdlist/Dos9_CmdList.c ./console/linux/Dos9_Console.c \
	./console/win/Dos9_Console.c ./dir/Dos9_Attr.c ./dir/Dos9_Dir.c ./enc/Dos9_Enc.c \
    ./estr/Dos9_Estr.c stack/Dos9_Stack.c ./thread/Dos9_Thread.c ./file/Dos9_StdFile.c \
	./file/Dos9_Win32File.c ./init/Dos9_Init.c
		
OBJ=$(SRC:.c=.o)

all: $(TARGET) $(INCLUDE_FILE)

$(INCLUDE_FILE): $(ORIGIN_FILE)
	cp $(ORIGIN_FILE) $(INCLUDE_FILE)

# build the main lib
$(TARGET): $(OBJ)
			$(AR) $(AFLAGS) $@ $^

%.o : %.c
			$(CC) -o $@ -c $< $(CFLAGS)


