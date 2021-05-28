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


ROOTDIR = ..
include ../config.mk

CFLAGS += $(MICROGETTEXT_INC) $(LIBPBAT_INC) \
          $(LIBMATHEVAL_INC) $(LIBCU8_INC)

LDFLAGS += $(MICROGETTEXT_LD) $(LIBPBAT_LD) \
           $(LIBMATHEVAL_LD) $(LIBCU8_LD) \
		   $(LPTHREAD_LD) $(LM_LD) $(LDL_LD)

ifeq ($(use_libcu8),1)
	LDFLAGS += -lshlwapi
endif

ifeq ($(fn_WIN32),1)
	SRC_RC = rc/res.rc
	OBJ_RC = rc/res.o
endif


SRC_LINENOISE = ./linenoise/pBat_LineNoise.c ./linenoise/utf8.c \
				./linenoise/linenoise.c 

ifeq ($(use_linenoise),0)
  SRC_LINENOISE =
endif

SRC_FILES := \
	./pBat.c ./core/pBat_Args.c ./core/pBat_Expand.c \
	./core/pBat_Globals.c ./core/pBat_FdInheritance.c \
	./core/pBat_Jump.c ./core/pBat_Run.c ./core/pBat_SplitPath.c \
	./core/pBat_ShowIntro.c ./core/pBat_ExitInt.c ./core/pBat_Var.c \
	./core/pBat_Parse.c ./core/pBat_Read.c ./core/pBat_Stream.c \
	./errors/pBat_Errors.c ./lang/pBat_Lang.c ./core/pBat_Debug.c \
	./core/pBat_FilePath.c ./core/pBat_ParseBlock.c ./lang/pBat_ShowHelp.c \
	./core/pBat_ExpandAlias.c ./core/pBat_ReadScript.c ./core/pBat_Clone.c \
	./command/pBat_Alias.c ./command/pBat_Ask.c \
	./command/pBat_Block.c ./command/pBat_Call.c ./command/pBat_Cd.c \
	./command/pBat_Cls.c ./command/pBat_Color.c ./command/pBat_Copy.c \
	./command/pBat_Del.c ./command/pBat_Dir.c ./command/pBat_Echo.c \
	./command/pBat_Exit.c ./command/pBat_For.c ./command/pBat_Goto.c \
	./command/pBat_Help.c ./command/pBat_If.c ./command/pBat_Mkdir.c \
	./command/pBat_Pause.c ./command/pBat_Rem.c \
	./command/pBat_Ren.c ./command/pBat_Rmdir.c ./command/pBat_Set.c \
	./command/pBat_Setlocal.c ./command/pBat_Shift.c ./command/pBat_Title.c \
	./command/pBat_Type.c ./command/pBat_Find.c ./core/pBat_Env.c \
	./command/pBat_More.c ./core/pBat_OsStream.c ./command/pBat_Start.c \
	./command/pBat_Break.c ./command/pBat_IfExp.c ./command/pBat_Chcp.c \
	./command/pBat_Pushd.c ./init/pBat_Init.c ./command/pBat_Wc.c \
	./command/pBat_Xargs.c ./core/pBat_DirStack.c ./core/pBat_EnvStack.c \
	./core/pBat_Exec.c ./core/pBat_Prompt.c ./command/pBat_Pecho.c \
	./command/pBat_CommandInfo.c ./core/pBat_Completion.c \
	./command/pBat_Timeout.c ./command/pBat_Mod.c \
	./command/pBat_Ver.c ./command/pBat_Locale.c $(SRC_LINENOISE) \

OBJ_FILES := $(SRC_FILES:.c=.o) $(OBJ_RC)

all: pbat

clean:
	rm -f $(OBJ_FILES)
	rm -f pbat$(EXEC_SUFFIX)

bin: pbat
	cp pbat$(EXEC_SUFFIX) $(BINDIR)/

pbat: $(OBJ_FILES)
	$(CC) -o pbat$(EXEC_SUFFIX) $(OBJ_FILES) $(LDFLAGS)
	
.rc.o:
	$(WINDRES) -o $@ -i $<

.SUFFIXES: .rc .o
.PHONY: all clean bin
