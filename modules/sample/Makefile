ROOTDIR = ../..
include $(ROOTDIR)/config.mk

SOURCES = Sample.c

OBJ_FILES = $(SOURCES:.c=.o) ../lib/pBat_Module.o

LDFLAGS += $(LIBPBAT_LD) 
CFLAGS += $(LIBPBAT_INC) -I../lib/

all: sample
bin: sample
	mkdir -p $(BINDIR)/modules
	cp sample$(EXEEXT) $(BINDIR)/modules/

clean:
	rm -rf $(OBJ_FILES)	
	
sample: $(OBJ_FILES)
	$(CC) -shared -fvisibility=hidden -o sample $(OBJ_FILES) $(LDFLAGS)
