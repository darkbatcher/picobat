#
#   pBat Makefiles, The pBat project
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

# microgettext
MICROGETTEXT_DIR = $(ROOTDIR)/microgettext

MICROGETTEXT_INC = -I$(MICROGETTEXT_DIR)
MICROGETTEXT_LD = -L$(MICROGETTEXT_DIR) -lmicrogettext

# libpBat
LIBPBAT_DIR = $(ROOTDIR)/libpBat

LIBPBAT_INC = -I$(LIBPBAT_DIR)
LIBPBAT_LD = -L$(LIBPBAT_DIR) -lpBat

# libmatheval
LIBMATHEVAL_DIR = $(ROOTDIR)/libmatheval

LIBMATHEVAL_INC = -I$(LIBMATHEVAL_DIR)/lib
LIBMATHEVAL_LD = -L$(LIBMATHEVAL_DIR) -lmatheval

ifneq (,$(wildcard $(ROOTDIR)/femto-config.mk))
    include $(ROOTDIR)/femto-config.mk
endif

#ifeq ($(fn_MINGW_W64),1)
#	CFLAGS += -D__USE_MINGW_ANSI_STDIO
#endif

ifeq ($(use_libcu8),1)
	LIBCU8_LD = -L$(ROOTDIR)/libcu8 -lcu8
	LIBCU8_INC = -I$(ROOTDIR)/libcu8/include
endif

ifeq ($(use_nls),1)
	ifeq ($(lib_intl),1)
		LINTL_LD = -lintl
	endif
endif

ifeq ($(fn_WIN32),0)
	ifeq ($(lib_pthread),1)
		LPTHREAD_LD = -lpthread
	endif

	ifeq ($(lib_m),1)
		LM_LD = -lm
	endif
endif

ifeq ($(use_modules),1)
	ifeq ($(lib_dl),1)
		LDL_LD = -ldl
	endif
	ifeq ($(flag_PIC),1)
		CFLAGS += -fPIC
		LDFLAGS += -fPIC
	endif
endif


