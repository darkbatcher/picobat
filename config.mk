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

# Get directory of config.mk

EXEC_SUFFIX ?=
OBJ_EXT ?= .o

LD = $(CC)
AR = ar r
RANLIB = ranlib

LEX ?= flex
YACC ?= yacc

CFLAGS ?= -O2
LDFLAGS ?= -O2 -s

BINDIR ?= $(ROOTDIR)/bin

# libDos9
LIBDOS9_DIR = $(ROOTDIR)/libDos9

LIBDOS9_INC = -I$(LIBDOS9_DIR)
LIBDOS9_LD = -L$(LIBDOS9_DIR) -lDos9

# libinteval
LIBINTEVAL_DIR = $(ROOTDIR)/libinteval

LIBINTEVAL_INC = -I$(LIBINTEVAL_DIR)
LIBINTEVAL_LD = -L$(LIBINTEVAL_DIR) -linteval

# libmatheval
LIBMATHEVAL_DIR = $(ROOTDIR)/libmatheval

LIBMATHEVAL_INC = -I$(LIBMATHEVAL_DIR)/lib
LIBMATHEVAL_LD = -L$(LIBMATHEVAL_DIR) -lmatheval

ifneq (,$(wildcard $(ROOTDIR)/femto-config.mk))
    include $(ROOTDIR)/femto-config.mk
endif

ifeq ($(use_libcu8),1)
	LIBCU8_LD = -L$(ROOTDIR)/libcu8 -lcu8
	LIBCU8_INC = -I$(ROOTDIR)/libcu8/include
endif

ifeq ($(lib_iconv),1)
	LICONV_LD = -liconv
endif

ifeq ($(lib_intl),1)
	LINTL_LD = -lintl
endif

ifeq ($(lib_pthread),1)
	LPTHREAD_LD = -lpthread
endif

ifeq ($(lib_m),1)
	LM_LD = -lm
endif

