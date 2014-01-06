# hlp_make , The dos9 project
# Copyright (c) 2013-2014 Romain Garbi (DarkBatcher)
#
# to set the target operating system set the evironment variable to either
# ``win'' or ``linux''. The default build is for windows
#
# to rebuild, use ``Make -B'' 

CC=gcc

SRC=hlp_make.c
OBJ=$(SRC:.c=.o)

LIB=$(LIB_DIR)/libDos9.a

TARGET=$(CMD_DIR)/hlp_make

ifeq ($(BUILD_OS),WINDOWS)

CFLAGS=-Wall -g -I $(INC_DIR)

else

CFLAGS=-Wall -g -I $(INC_DIR) -pthread

endif

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)
	
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<