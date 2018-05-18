# Copyright (c) 2014, 2015, 2016 Romain GARBI
# 
# All rights reserved.
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
# 
# * Redistributions of source code must retain the above copyright
#   notice, this list of conditions and the following disclaimer.
# * Redistributions in binary form must reproduce the above copyright
#   notice, this list of conditions and the following disclaimer in the
#   documentation and/or other materials provided with the distribution.
# * Neither the name of the name of Romain Garbi (Darkbatcher) nor the
#   names of its contributors may be used to endorse or promote products
#   derived from this software without specific prior written permission.
# 
# THIS SOFTWARE IS PROVIDED BY ROMAIN GARBI AND CONTRIBUTORS ``AS IS'' AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL ROMAIN GARBI AND CONTRIBUTORS BE LIABLE FOR ANY
# DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
# (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
# LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

ifneq (,$(wildcard ../femto-config.mk))
    include ../femto-config.mk
endif

ifdef lib_iconv
	LDFLAGS += -liconv
endif

noinst_SOURCES = convert.c ansi.c invalid.c read.c read850.c readconsole.c 
noinst_PROGRAMS = $(noinst_SOURCES:.c=)

ansi_SOURCES = ansi.c
ansi_LDADD = -L.. -lcu8
ansi_CFLAGS = -I../include

convert_SOURCES = convert.c
convert_LDADD = -L.. -lcu8
convert_CFLAGS = -I../include

read_SOURCES = read.c
read_LDADD =  -L.. -lcu8 
read_CFLAGS = -I../include

read850_SOURCES = read850.c 
read850_LDADD =  -L.. -lcu8
read850_CFLAGS = -I../include

readconsole_SOURCES = readconsole.c
readconsole_LDADD = -L.. -lcu8
readconsole_CFLAGS = -I../include

invalid_SOURCES = invalid.c
invalid_LDADD = -L.. -lcu8
invalid_CFLAGS = -I../include

all: $(noinst_PROGRAMS)

$(noinst_PROGRAMS): $(noinst_SOURCES) ../libcu8.a
	$(CC) -o $@$(EXEC_SUFFIX) $($@_SOURCES) $($@_CFLAGS) $($@_LDADD) $(CFLAGS) $(LDFLAGS) 
	
clean:
	rm -f *.o
	rm -f $(addsuffix $(EXEC_SUFFIX),$(noinst_PROGRAMS)) 	

# Run the tests
run-tests: all
	@for i in $(noinst_PROGRAMS); do \
		echo -n "Running test ./$$i... " ; \
		./$$i ; \
		case $$? in \
			0) echo  "Ok";; \
			*) echo "Failed"; \
			echo ERROR: `[ -f log_msg ] && cat log_msg`; \
			exit -1;; \
		esac;\
	done; 2>&1

.PHONY: run-tests all clean