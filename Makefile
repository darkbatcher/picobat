# Overall Dos9 suite Makefile, The Dos9 Project (c) Darkbatcher
# 2010-2013
#
# This is free software: you can redistribute it and/or modify it under the
# terms of the GNU General Public License as published by the Free Software 
# Foundation, either version 3 of the License, or (at your option) any later
# version.
#

# Define the location of output binaries
# will be put
BIN=./bin
BIN_CMD=./bin/cmd

# Defines the location of generated libraries
EXT=.exe

# Defines the files to be build, in order
# of building
	  
all:
# first of all, we need to produce the library
	make -C ./libdos9
# then, we produce Dos9 itself
	make -C ./dos9
# and the utilites
	make -C ./tea
	make -C ./tea_html
	make -C ./hlp
	make -C ./hlp_make
	
