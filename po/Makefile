# Overall pBat suite Makefile, The pBat Project (c) Darkbatcher
# 2010-2016
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
#
# This files generates translations while compiling
#

ROOTDIR=..
BINDIR ?= bin

MSGFMT = msgfmt
XGETTEXT = xgettext
MSGMERGE = msgmerge
MSGCAT = msgcat
MSGINIT = msginit

LOCALES = $(shell cat locales.list) 
DOMAINS = $(shell awk '{print $$1}' < domains.list)
SOURCES = $(shell awk '{$$1 = ""; print $$0}' < domains.list)
POTS = $(SOURCES:.c=.pot)
POS = $(shell for i in $(LOCALES); do for j in $(DOMAINS); do echo $$i/$$j.po; done; done)
MOS = $(POS:.po=.mo)
DOMAIN_POTS = $(POS:.po=.pot)
BIN_LOCALES = $(addsuffix .bin,$(LOCALES))
LOCALE_DIR = $(ROOTDIR)/$(BINDIR)/share/locale


all: $(LOCALES) $(MOS)

bin: all $(BIN_LOCALES)
	
$(BIN_LOCALES):	
	[ -f $(LOCALE_DIR)/$(basename $@)/LC_MESSAGES ] || mkdir -p $(LOCALE_DIR)/$(basename $@)/LC_MESSAGES
	cp $(addprefix $(basename $@)/,$(addsuffix .mo,$(DOMAINS))) $(LOCALE_DIR)/$(basename $@)/LC_MESSAGES
	
clean:
	rm -rf $(DOMAIN_POTS)
	rm -rf $(POTS)
	
$(LOCALES):
	mkdir $@	
	
$(DOMAIN_POTS): $(POTS)
	$(MSGCAT) -t UTF-8 $(shell grep -e "^$(shell basename $(basename $@))"  < domains.list | awk '{$$1 = ""; print $$0}' | sed -e s,\.c,\.pot,g ) > $@ || true
	
.c.pot:
	$(XGETTEXT) -o $@ $<
		
.pot.po:
	[ -f $@ ] && $(MSGMERGE) -U  $@ $< || $(MSGINIT) -o $@ -i $<
	touch $@

.po.mo:
	$(MSGFMT) -c -v -o $@ $<
	
.PHONY: all clean bin $(BIN_LOCALES)
.SUFFIXES: .c .pot .mo .po .pot
.SECONDARY: $(POS)
