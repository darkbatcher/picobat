ROOTDIR = ../..
include $(ROOTDIR)/config.mk

SOURCES = Sample.c

OBJ_FILES = $(SOURCES:.c=.o) ../lib/Dos9_Module.o

LDFLAGS += $(LIBDOS9_LD) 
CFLAGS += $(LIBDOS9_INC) -I../lib/

all: sample
bin: sample
	cp sample$(EXEEXT) $(BINDIR)/modules/

clean:
	rm -rf $(OBJ_FILES)	
	
sample: $(OBJ_FILES)
	$(CC) -shared -fvisibility=hidden -o sample $(OBJ_FILES) $(LDFLAGS)
