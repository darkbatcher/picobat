# hlp_make , The dos9 project (c) darkbatcher 2013
#
# to set the target operating system set the evironment variable to either
# ``win'' or ``linux''. The default build is for windows
#
# to rebuild, use ``Make -B'' 

CC=gcc

SRC=hlp_make.c
OBJ=$(SRC:.c=.o)

LIB=../libs/libDos9.a

TARGET=../bin/cmd/hlp_make
CFLAGS=-Wall -g -pthread -I ../libdos9

all: $(TARGET)

$(TARGET) : $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^ $(LIB)
	
%.o: %.c
	$(CC) $(CFLAGS) -o $@ -c $<