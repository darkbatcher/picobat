#
#   Dos9 Makefiles, The Dos9 project
#   Copyright (C) 2012-2018  Romain Garbi, Teddy Astie
#
#   This program is free software: you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation, either version 3 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

ifneq (,$(wildcard ./femto-config.mk))
    include femto-config.mk
endif

BINDIR ?= ./bin

SUBDIRS_ADD ?=

ifdef use_libcu8
	SUBDIRS_ADD = libcu8
endif

SUBDIRS = libDos9 libinteval libmatheval $(SUBDIRS_ADD) dos9 dos9ize dump tea scripts po
TEAFILES = README.tea WHATSNEW.tea GUIDELINES.tea THANKS.tea
TEXTFILES = $(TEAFILES:.tea=)

SUBDIRS_CLEAN := $(addsuffix .clean,$(SUBDIRS))
SUBDIRS_BIN := $(addsuffix .bin,$(SUBDIRS))

HOST = $(shell $(CC) -dumpmachine)
YEAR = $(shell date +%Y)
			
all: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@
	
po-merge:
	$(MAKE) -C po po-merge

clean: $(SUBDIRS_CLEAN)
	rm -f $(TEXTFILES)

$(SUBDIRS_CLEAN):
	$(MAKE) -C $(basename $@) clean
	
bin: bindir $(SUBDIRS_BIN)

bindir: $(TEXTFILES)
	mkdir -p $(BINDIR)
	mkdir -p $(BINDIR)/cmd
	cp -r ./po/share $(BINDIR)
	cp README $(BINDIR)/
	cp WHATSNEW $(BINDIR)/
	cp COPYING $(BINDIR)/
	cp COPYING.darkbox $(BINDIR)/
	cp libmatheval/COPYING $(BINDIR)/COPYING.libmatheval
	cp THANKS $(BINDIR)/
	
$(TEXTFILES): $(TEAFILES)
	tea/tea -e:utf-8 -o:text-plain $@.tea $@
	
$(SUBDIRS_BIN): $(SUBDIRS)
	$(MAKE) -C $(basename $@) bin || true
	
# stuff to check
PROGRAMS = xdg-open
FUNCTIONS =
LIBS = iconv intl pthread
OPTIONS = libcu8 nls cmdlycorrect console
DEFAULTOPTIONS = no-libcu8 use-nls no-cmdlycorrect use-console
SUBCONFIG = libcu8
ADDITIONALVARS = HOST BINDIR YEAR

include femto.mk

.PHONY: all bin clean $(SUBDIRS) $(SUBDIRS_CLEAN) po-merge