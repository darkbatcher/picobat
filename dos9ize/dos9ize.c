/*
 *
 *   dos9ize, a free Dos9 batch adapter, The Dos9 Project
 *   Copyright (C) 2010-2016 Romain GARBI
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */
#include <stdio.h>
#include <stdlib.h>

#include <libDos9.h>

#include "dos9ize.h"
/* dos9ize [/c config] input [output]

    Do some adjustments to batch inputs to make them running using Dos9.

	- input: file from which the command will get its input
	
	- output : file to which the ouput will be written
	
	- /c config : path of a configuration file used to define command and switches
    associated to them. The default path for the configuration file is
	~/.dos9/dos9ize.lst or /etc/dos9/dos9ize.lst for unices and
	%USERPROFILE%/.dos9/dos9ize.lst or in the same directory as the command
	executalble

    Adjustments performed are :

        - Adding spaces between switches
        - Removing ^^^! in favor of ^!

*/

#define ERROR(...) do { \
	fprintf(stderr, "dos9ize: "); \
	fprintf(stderr, __VA_ARGS__ ); \
	exit(-1); \
	} while (0)

command commands[] = {
    {"RD", "SQ"},
    {"RMDIR", "SQ"},
    {"CD", "D"},
    {"CHDIR", "D"},
    {"COPY", "RYA"},
    {"DEL", "PFSQA"},
    {"ERASE", "PFSQA"},
    {"DIR", "ASB"},
    {"FOR", "FLRD"},
    {"IF" , "I"},
    {"SET", "AP"},
    {"EXIT", "B"},
    {"FIND", "NCVIE"},
    {"MORE", "EPSCT"},
    {NULL, NULL}
};

/* Get the content of the file */
void read_file(FILE* file, ESTR* content)
{
	ESTR* tmp = Dos9_EsInit();
	
	while(!Dos9_EsGet(tmp, file))
		Dos9_EsCatE(content, tmp);

	Dos9_EsFree(tmp);
}

int main(int argc, char* argv[])
{
    FILE* input;
	FILE* output;
	pair p;
    int i = 0;

    ESTR* toreplace = Dos9_EsInit();
    ESTR* replaceby = Dos9_EsInit();
	ESTR* content = Dos9_EsInit();

	/* Check command line integrity */
	if (argc <= 1)
		ERROR("Bad command line\n");

	/* Open input and output files */
	if (!(input = fopen(argv[1], "r")))
		ERROR("Unable to open \"%s\".\n", argv[1]);

	if (argc > 2) {
	
		if (!(output = fopen(argv[2], "w+")))
			ERROR("Unable to open \"%s\".\n", argv[2]);
	
	} else {

		output = stdout;
	
	}
	
	/* Get the content of the input file */
	read_file(input, content);
	fclose(input);

    /* First, force the use of spaces between switches */
    while (commands[i].cmd != NULL) {

        pair_start(&p, commands+i);

        do {

            pair_replace_exps(&p, toreplace, replaceby);
			Dos9_EsReplaceI(content, toreplace->str, replaceby->str);

        } while (pair_next(&p));

        i++;

    }
	
    /* Next replace exessive numbers of escaping character (^) */

	fprintf(output, "%s", content->str);
	fclose(output);

    Dos9_EsFree(replaceby);
    Dos9_EsFree(toreplace);

    return 0;
}
