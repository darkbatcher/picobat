/*
 *
 *   TEA - A quick and simple text preprocessor
 *   Copyright (C) 2010-2016 DarkBatcher
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
#include <errno.h>
#include <string.h>

#include <libDos9.h>

#include "Tea.h"
#include "out/tea_out.h"


ESTR* lpEsEncoding;

int main(int argc, char *argv[])
{
	TEAPAGE* lpTeaPage;
	LP_OUTPUT_HANDLER pOutputHandler=Tea_TextOutputHandler;
	LP_PARSE_HANDLER pParseHandler=Tea_TextParseHandler;

	if (Dos9_LibInit() == -1) {

		puts("Error : Unable to load LibDos9. Exiting ...");
		exit(-1);

	}

	FILE* pOutput;

	char* lpPagePath=NULL;
	char* lpOutputPath=NULL;

	int i=1,
	    iContinue=TRUE;

	lpEsEncoding=Dos9_EsInit();
	Dos9_EsCpy(lpEsEncoding, "US-ASCII");

	if (argc < 1) {
		fputs("TEA :: Error : Invalid command line\n", stderr);
		return -1;
	}

	Dos9_SetNewLineMode(DOS9_NEWLINE_LINUX);

	while (argv[i] && iContinue) {

		if (!strnicmp("/O", argv[i], 2)
            || !strnicmp("-O", argv[i], 2)) {

			argv[i]+=2;
			if (*(argv[i])==':') argv[i]++;

			if (!stricmp("TEXT", argv[i])) {

				pOutputHandler=Tea_TextOutputHandler;
				pParseHandler=Tea_TextParseHandler;

			} else if (!stricmp("HTML", argv[i])) {

				pOutputHandler=Tea_HtmlOutputHandler;
				pParseHandler=Tea_HtmlParseHandler;

			} else if (!stricmp("TEXT-PLAIN", argv[i])) {

				pOutputHandler=Tea_TextOutputHandler;
				pParseHandler=Tea_TextPlainParseHandler;

			} else if (!stricmp("TEXT-ANSI", argv[i])) {

				pOutputHandler=Tea_TextAnsiOutputHandler;
				pParseHandler=Tea_TextAnsiParseHandler;

            } else if(!stricmp("MD", argv[i])) {

                pOutputHandler = Tea_MDOutputHandler;
                pParseHandler = Tea_MDParseHandler;

			} else {

				fprintf(stderr, "TEA :: Error : ``%s'' is not a valid output format.\n",  argv[i]);
				exit(-1);
			}

		} else if (!strnicmp("/E", argv[i], 2)
                   || !strnicmp("-E", argv[i], 2)) {

			argv[i]+=2;
			if (*(argv[i])==':') argv[i]++;

			if (!stricmp("UTF-8" ,argv[i])) {

				Dos9_SetEncoding(DOS9_UTF8_ENCODING);

			} else {

				Dos9_SetEncoding(DOS9_BYTE_ENCODING);

			}

			Dos9_EsCpy(lpEsEncoding, argv[i]);

		} else if (!strcmp("/?", argv[i])) {

			puts("Tea text preprocessor - version 1\n"
                 "Copyright (C) 2010-2018 Romain Garbi.\n"
                 "Distributed under the terms of the GPL v3");
			return 0;

		} else {

			if (!lpPagePath) {

				lpPagePath=argv[i];

			} else if (!lpOutputPath) {

				lpOutputPath=argv[i];

			} else {

				iContinue=FALSE;
				continue;

			}

		}

		i++;
	}

	if (!(lpTeaPage=Tea_PageLoad(lpPagePath, pParseHandler))) {

		printf("TEA :: Error : Unable to open TEA script ``%s'' : %s", lpPagePath, strerror(errno));
		return errno;

	}

	if (lpOutputPath) {

		if (!(pOutput=fopen(lpOutputPath, "w+"))) {

			printf("TEA :: Error : Unable to open ``%s'' as output : %s", lpOutputPath, strerror(errno));
			return errno;

		}

	} else {

		pOutput=stdout;

	}

	pOutputHandler(lpTeaPage, pOutput, i, argv);

	Tea_PageFree(lpTeaPage);

	fclose(pOutput);

	Dos9_LibClose();

	return 0;
}
