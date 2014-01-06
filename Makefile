# Overall Dos9 suite Makefile, The Dos9 Project (c) Darkbatcher
# 2010-2013
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#

# define a list of components to be build
DOS9_COMPONENTS=./libDos9 ./libmatheval-dos9 ./libinteval ./dos9 ./tea ./tea_html \
				./hlp ./hlp_make
				
# define first part of build bash scripts
DOS9_VAR_EXPORT= export LIB_DIR=../libs; \
				 export CMD_DIR=../bin/cmd; \
				 export BIN_DIR=../bin; \
				 export INC_DIR=../include;
	
all:
	# Dos9 overall makefile.
	# Copyright (c) 2010 - 2013 Darkbatcher
	#
	# This is free software : you can redistribute it and/or modify it under the
	# terms of the GNU General Public License as published by the Free Software
	# Foundation, either version 3 of the License, or (at your option) any later
	# version.
	#
	# Dos9 have different build scripts defined for different existing operating 
	# systems.
	#
	# - Select ``win'' target for building for windows.
	#	
	#       Using the ``win'' target supposes that the standard win32 api is
	#       available for applications. It also suppose that your system support
	#       POSIX function of MSVCRT. The target also suppose that you have a
	#       standard *nix shell installed on your system \(sh or other\). Such
	#       systems can be obtained through MSYS from the MinGW Project.
	#
	# - Select ``*nix'' target for building for *nix (ie. UNIX based operating
	#   systems like GNU/Linux, GNU/Hurd, BSD, and so on).
	#
	#       Using the ``*nix'' target suposses that your system support basic
	#       POSIX api, and have aready GNU gettext installed in your system.
	#
	# - Select ``doc'' target for building documentation
	#
	#       Using ``doc'' target supposes you have already built Dos9 executables
	#
	# Both ``*nix'' and ``win'' build require a C-compiler (Obviously) and the
	# Lex/Yacc *nix tools. By default, the C-compiler is gcc and the Lex/Yacc tools
	# are GNU Flex and GNU Bison. Anyway, this setting might be overriden by 
	# setting the following variables and using -e option with make:
	#
	#       - CC : The name of the compiler to be used.
	#
	#       - LEX : The name of the Lex tool to be used
	#
	#       - YACC : The name of the Yacc tool to be used
	#
	#       - CFLAGS : The compiler flags. However, this is quite difficult since
	#       flags depends on many thing. See Makefiles to get more informations
	#
	# Thank you for using Dos9.
	
# make for WINDOWS target
win:
	@$(DOS9_VAR_EXPORT)\
	export BUILD_OS=WINDOWS;\
	for i in $(DOS9_COMPONENTS); do $(MAKE) -e -C $$i ; done;
	
# make for NIX targets
*nix:
	@$(DOS9_VAR_EXPORT)\
	export BUILD_OS=*NIX;\
	for i in $(DOS9_COMPONENTS); do $(MAKE) -e -C $$i ; done;

# make the documentation	
doc:
	./make-doc.bat
	
.PHONY: clean

clean:
	rm -r *.o
