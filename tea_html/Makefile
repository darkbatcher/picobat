# hlp_make , The dos9 project (c) darkbatcher 2013
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Please report bugs, and mistakes at : <darkbatcher at dos9 dot org>
#

CC=gcc

SRC=tea_html.c tea_html_funcs.c
OBJ=$(SRC:.c=.o)

LIB=../$(LIB_DIR)/libDos9.a

TARGET=../$(CMD_DIR)/tea_html

ifeq ($(BUILD_OS),WINDOWS)

CFLAGS=-Wall -g -I ../$(INC_DIR)

else

CFLAGS=-Wall -g -pthread -I ../$(INC_DIR)

endif

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)

%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<