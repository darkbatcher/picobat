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

ROOTDIR=..
BINDIR?=$(ROOTDIR)/bin
DOS9=$(BINDIR)/dos9
TESTS=$(shell find . -name "*.bat")
TESTS.OUT=$(TESTS:.bat=.out)


# do nothing
all:

test: test-clean $(TESTS.OUT)

test-clean:
	rm -Rf $(TESTS.OUT)

.bat.out:
	@echo Running test :  $<
	@$(DOS9) run-test.cmd $< $@
	@cmp $@ $@.ok; \
	case $$? in \
		0) echo "	OK";true;; \
		*) echo "	ERROR";false;; \
	esac;
.PHONY: test test-clean
.SUFFIXES: .bat .out