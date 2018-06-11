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


ROOTDIR = ..
include ../config.mk

CFLAGS += $(LIBDOS9_INC) $(LIBINTEVAL_INC) $(LIBMATHEVAL_INC) \
		  $(LIBCU8_INC)

LDFLAGS += $(LIBDOS9_LD) $(LIBINTEVAL_LD) $(LIBMATHEVAL_LD) \
		   $(LIBCU8_LD) $(LICONV_LD)  $(LINTL_LD) $(LPTHREAD_LD)  \
		   $(LM_LD)
		   
ifeq ($(use_libcu8),1)
	LDFLAGS += -lshlwapi
endif
		  

SRC_FILES := \
	./Dos9.c ./core/Dos9_Args.c ./core/Dos9_Expand.c \
	./core/Dos9_Globals.c ./core/Dos9_FdInheritance.c \
	./core/Dos9_Jump.c ./core/Dos9_Run.c ./core/Dos9_SplitPath.c \
	./core/Dos9_ShowIntro.c ./core/Dos9_ExitInt.c ./core/Dos9_Var.c \
	./core/Dos9_Parse.c ./core/Dos9_Read.c ./core/Dos9_Stream.c \
	./errors/Dos9_Errors.c ./lang/Dos9_Lang.c ./core/Dos9_Debug.c \
	./core/Dos9_FilePath.c ./core/Dos9_ParseBlock.c ./lang/Dos9_ShowHelp.c \
	./core/Dos9_ExpandAlias.c ./core/Dos9_ReadScript.c ./core/Dos9_Clone.c \
	./command/Dos9_Alias.c ./command/Dos9_Ask.c \
	./command/Dos9_Block.c ./command/Dos9_Call.c ./command/Dos9_Cd.c \
	./command/Dos9_Cls.c ./command/Dos9_Color.c ./command/Dos9_Copy.c \
	./command/Dos9_Del.c ./command/Dos9_Dir.c ./command/Dos9_Echo.c \
	./command/Dos9_Exit.c ./command/Dos9_For.c ./command/Dos9_Goto.c \
	./command/Dos9_Help.c ./command/Dos9_If.c ./command/Dos9_Mkdir.c \
	./command/Dos9_Pause.c ./command/Dos9_Rem.c \
	./command/Dos9_Ren.c ./command/Dos9_Rmdir.c ./command/Dos9_Set.c \
	./command/Dos9_Setlocal.c ./command/Dos9_Shift.c ./command/Dos9_Title.c \
	./command/Dos9_Type.c ./command/Dos9_Find.c ./core/Dos9_Env.c \
	./command/Dos9_More.c ./core/Dos9_OsStream.c ./command/Dos9_Start.c \
	./command/Dos9_Break.c ./command/Dos9_IfExp.c ./command/Dos9_Chcp.c \
	./command/Dos9_Pushd.c ./init/Dos9_Init.c ./command/Dos9_Wc.c \
	./command/Dos9_Xargs.c ./core/Dos9_DirStack.c ./core/Dos9_EnvStack.c \
	./core/Dos9_Exec.c ./console/Dos9_ConsoleANSI.c \
	./console/Dos9_ConsoleWin.c ./linenoise/Dos9_LineNoise.c

OBJ_FILES := $(SRC_FILES:.c=.o)

all: dos9

clean:
	rm -f $(OBJ_FILES)
	rm -f dos9$(EXEC_SUFFIX)

bin: dos9
	cp dos9$(EXEC_SUFFIX) $(BINDIR)/

dos9: $(OBJ_FILES)
	$(CC) -o dos9$(EXEC_SUFFIX) $(OBJ_FILES) $(LDFLAGS)

.PHONY: all clean bin
