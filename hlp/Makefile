# Hlp, The Dos9 Project
# Copyright © 2010-2014 Darkbatcher
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Please report bugs, and mistakes at : <darkbatcher at dos9 dot org>
#

# the compilers vars
CC=gcc

#the list of the sources
SRC= ./src/hlp.c ./src/hlp_show.c ./src/hlp_load.c ./src/hlp_enc.c 

ifeq ($(BUILD_OS),WINDOWS)
	
LIB=$(LIB_DIR)/libDos9.a $(LIB_DIR)/libiconv.dll.a $(LIB_DIR)/libintl.lib

else

LIB=$(LIB_DIR)/libDos9.a 

endif

TARGET=$(CMD_DIR)/hlp
CFLAGS=-Wall -g -I $(INC_DIR)

OBJ=$(SRC:.c=.o)

# build all
all: $(TARGET)

# build the main lib
$(TARGET) : $(OBJ)
			$(CC) $(CFLAGS) -o $@ $^ $(LIB)

%.o : %.c
			$(CC) -o $@ -c $< $(CFLAGS)
