ROOTDIR = ../..
include $(ROOTDIR)/config.mk

MODULE = batbox
SOURCES = batbox.c

OBJ_FILES = $(SOURCES:.c=.o) ../lib/Dos9_Module.o

LDFLAGS += $(LIBDOS9_LD) 
CFLAGS += $(LIBDOS9_INC) -I../lib/

all: $(MODULE)

bin: $(MODULE)
	mkdir -p $(BINDIR)/modules
	cp $(MODULE)$(EXEEXT) $(BINDIR)/modules/

clean:
	rm -rf $(OBJ_FILES)	
	
$(MODULE)$(EXEEXT): $(OBJ_FILES)
	$(CC) -shared -fvisibility=hidden -o $(MODULE)$(EXEEXT) $(OBJ_FILES) $(LDFLAGS)

.PHONY: all clean bin