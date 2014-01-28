# Tea, The Dos9 Project - Copyright © 2010-2014 Darkbatcher
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
SRC= ./src/tea.c ./src/out/tea_html_out.c \
     ./src/Tea_Load.c ./src/out/tea_text_out.c ./src/Tea_Node.c \
     ./src/Tea_Output.c ./src/tea_strupr.c

#the list of the libs
LIB=$(LIB_DIR)/libDos9.a 

TARGET=$(CMD_DIR)/tea

ifeq ($(BUILD_OS),WINDOWS)

CFLAGS=-Wall -g -I $(INC_DIR)

else
 
CFLAGS=-Wall -g -pthread -I $(INC_DIR)

endif

OBJ=$(SRC:.c=.o)

# build all
all: $(TARGET)

$(TARGET) : $(OBJ)
			$(CC) $(CFLAGS) -s -o $@ $^ $(LIB)

%.o : %.c
			$(CC) -o $@ -c $< $(CFLAGS)