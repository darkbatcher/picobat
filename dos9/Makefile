# Dos9, The Dos9 Project - Copyright © 2010-2013 Darkbatcher
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#
# Please report bugs, and mistakes at : <darkbatcher at dos9 dot org>

# the compilers vars
CC=gcc

#the list of the sources
SRC=./Dos9.c ./command/Dos9_Conditions.c ./command/Dos9_FileCommands.c \
	./command/Dos9_ScriptCommands.c ./command/cmdlib/Dos9_CmdLibArgs.c \
	./core/Dos9_Expand.c ./core/Dos9_Globals.c ./core/Dos9_Jump.c \
	./core/Dos9_Run.c ./core/Dos9_SplitPath.c ./core/Dos9_ShowIntro.c \
	./core/Dos9_Exit.c ./core/Dos9_Var.c ./command/Dos9_For.c \
	./core/Modules/Parse/Dos9_Parse.c ./core/Modules/Read/Dos9_Read.c \
	./core/Stream/Dos9_Stream.c ./errors/Dos9_Errors.c \
	./lang/Dos9_Lang.c ./core/Dos9_Debug.c ./core/Dos9_FilePath.c

ifeq ($(BUILD_OS),WINDOWS)

# libs for windows
LIB=$(LIB_DIR)/libDos9.a $(LIB_DIR)/libinteval.a $(LIB_DIR)/libmatheval.a \
	 -lfl -lintl 
	
CFLAGS=-Wall -g -I $(INC_DIR)

else

# libs for *nix
LIB=$(LIB_DIR)/libDos9.a $(LIB_DIR)/libmatheval.a $(LIB_DIR)/libinteval -lfl -lm
CFLAGS=-Wall -g -pthread -I $(INC_DIR)
endif
	
TARGET=$(BIN_DIR)/Dos9

OBJ=$(SRC:.c=.o)

# build all
all: $(TARGET)

# build the main lib
$(TARGET) : $(OBJ)
			$(CC) $(CFLAGS) -o $@ $^ $(LIB)

%.o : %.c
			$(CC) -o $@ -c $< $(CFLAGS)
