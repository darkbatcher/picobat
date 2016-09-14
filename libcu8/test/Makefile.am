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
ACLOCAL_AMFLAGS = -I m4
AUTOMAKE_OPTIONS = foreign subdir-objects
EXTRA_DIST = check_u8 check_850

noinst_PROGRAMS = convert ansi read read850 readconsole

ansi_SOURCES = ansi.c log_msg.c
ansi_LDADD = ../src/libcu8.la
ansi_CFLAGS = -I../include

convert_SOURCES = convert.c log_msg.c
convert_LDADD = ../src/libcu8.la
convert_CFLAGS = -I../include

read_SOURCES = read.c log_msg.c
read_LDADD = ../src/libcu8.la
read_CFLAGS = -I../include

read850_SOURCES = read850.c log_msg.c
read850_LDADD = ../src/libcu8.la
read850_CFLAGS = -I../include

readconsole_SOURCES = readconsole.c log_msg.c
readconsole_LDADD = ../src/libcu8.la
readconsole_CFLAGS = -I../include

# Run the tests
run-tests: 
	@for i in $(noinst_PROGRAMS); do \
		echo $(ECHO_N) "Running test ./$$i... " $(ECHO_C); \
		./$$i ; \
		case $$? in \
			0) echo $(ECHO_T) "Ok";; \
			*) echo $(ECHO_T) "Failed"; \
			echo ERROR: `[ -f log_msg ] && cat log_msg`; \
			exit -1;; \
		esac;\
	done; 2>&1
