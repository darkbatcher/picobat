/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <libDos9.h>

#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

PARSED_LINE* Dos9_ParseLine(ESTR* lpesLine)
{
	PARSED_LINE *item, *begin;

	if (!(begin=Dos9_ParseOperators(lpesLine)))
		return NULL;

	item = begin;

    while (item) {

        if (!(item->sStream = Dos9_ParseOutput(item->lpCmdLine))) {

            Dos9_FreeParsedLine(begin);
            return NULL;

        }

        item = item->lppsNode;

    }

	return begin;
}

PARSED_STREAM* Dos9_ParseOutput(ESTR* lpesLine)
{

	char *lpCh=Dos9_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char lpCorrect[]="1";
	char cChar;

	ESTR *lpesFinal=Dos9_EsInit(),
	      *lpesParam=Dos9_EsInit();

	PARSED_STREAM* lppssStart;

	if (!(lppssStart=Dos9_AllocParsedStream())) {

		Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
		                      __FILE__ "/ParseOutput()",
		                      -1);
		goto error;

	}

	lpSearchBegin=lpCh;
	lpNextBlock=Dos9_GetNextBlockBegin(lpCh);

	while ((lpNextToken=Dos9_SearchToken_OutQuotes(lpSearchBegin, "12<>"))) {

		if ((lpNextToken >= lpNextBlock)
		    && (lpNextBlock!=NULL)) {

			lpSearchBegin=Dos9_GetNextBlockEnd(lpNextBlock);
            if (lpSearchBegin==NULL) {

				Dos9_FreeParsedStream(lppssStart);

				Dos9_ShowErrorMessage(DOS9_MALFORMED_BLOCKS, lpCh, FALSE);


				goto error;

			}

			lpNextBlock=Dos9_GetNextBlockBeginEx(lpSearchBegin+1, 1);

			continue;

		}

		cChar=*lpNextToken;

		*lpNextToken='\0';

		Dos9_EsCat(lpesFinal, lpCh);

		lpNextToken++;

		switch(cChar) {

		case '2':
			/* test wether this is the beginning of the token
			   '2>&1'
			*/

			if (!strncmp(lpNextToken, ">&1", 3)) {

				if (lppssStart->cRedir) {

                    Dos9_FreeParsedStream(lppssStart);

                    Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED, "1>&2", FALSE);

                    goto error;

				}

				lppssStart->cRedir =  PARSED_STREAM_STDERR2STDOUT;
				lpCh=lpNextToken+3;

				break;

			}

		case '1':

            if (!strncmp(lpNextToken, ">&2", 3)) {

				if (lppssStart->cRedir) {

                    Dos9_FreeParsedStream(lppssStart);

                    Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED, "1>&2", FALSE);

                    goto error;

				}

				lppssStart->cRedir = PARSED_STREAM_STDOUT2STDERR;

				lpCh=lpNextToken+3;

				break;

			}

			if (*lpNextToken!='>') {

				lpCorrect[0]=cChar;

				Dos9_EsCat(lpesFinal, lpCorrect);

				lpCh=lpNextToken;

				break;

			}

			lpNextToken++;

		case '>' :
			/* this is ouput */

			if ((cChar != '2' && lppssStart->lpOutputFile)
                || (cChar == '2' && lppssStart->lpErrorFile)) {

				Dos9_FreeParsedStream(lppssStart);

				Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED, lpNextToken, FALSE);

				goto error;


			}

			if (*lpNextToken!='>') {

				if (cChar == '2')
                    lppssStart->cErrorMode|=PARSED_STREAM_MODE_TRUNCATE;
                else
                    lppssStart->cOutputMode|=PARSED_STREAM_MODE_TRUNCATE;

			} else {

				lpNextToken++;

			}

			if (!(lpCh=Dos9_GetNextParameterEsD(lpNextToken, lpesParam,
                                                    DOS9_DELIMITERS "&|><"))) {

				Dos9_FreeParsedStream(lppssStart);

				Dos9_ShowErrorMessage(DOS9_INVALID_REDIRECTION, lpNextToken, FALSE);

				goto error;

			}

			if ((cChar != '2' && !(lppssStart->lpOutputFile =
                                Dos9_FullPathDup(Dos9_EsToChar(lpesParam))))
                || (cChar == '2' && !(lppssStart->lpErrorFile =
                                Dos9_FullPathDup(Dos9_EsToChar(lpesParam))))) {

				Dos9_FreeParsedStream(lppssStart);

				Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
				                      __FILE__ "/ParseOutput()",
				                      -1);

				goto error;

			}


			break;

		case '<' :

			if (lppssStart->lpInputFile) {

				Dos9_FreeParsedStream(lppssStart);

				Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED, lpNextToken, FALSE);

				goto error;


			}

			if (!(lpCh=Dos9_GetNextParameterEsD(lpNextToken, lpesParam,
                                                 DOS9_DELIMITERS "&|><"))) {

				Dos9_FreeParsedStream(lppssStart);

				Dos9_ShowErrorMessage(DOS9_INVALID_REDIRECTION, lpNextToken, FALSE);

				goto error;

			}

			if (!(lppssStart->lpInputFile=Dos9_FullPathDup(Dos9_EsToChar(lpesParam)))) {

				Dos9_FreeParsedStream(lppssStart);

				Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
				                      __FILE__ "/ParseOutput()",
				                      -1);

				goto error;

			}

		}

		lpSearchBegin=lpCh;

	}

	Dos9_EsCat(lpesFinal, lpCh);
	Dos9_EsCpyE(lpesLine, lpesFinal);


	Dos9_EsFree(lpesFinal);
	Dos9_EsFree(lpesParam);

	return lppssStart;

error:
	/* if some fail happened, free memory. However, the
	   origin string will be useless */
	Dos9_EsFree(lpesFinal);
	Dos9_EsFree(lpesParam);
	return NULL;

}

PARSED_LINE*       Dos9_ParseOperators(ESTR* lpesLine)
{
	PARSED_LINE *lppsStream=NULL,
					*lppsStreamBegin=NULL;

	char *lpCh=Dos9_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char cChar,
	     cNodeType=PARSED_STREAM_NODE_NONE;

	if (!(lppsStreamBegin=Dos9_AllocParsedLine(NULL))) {

		Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
		                      __FILE__ "/Dos9_ParseOperators()",
		                      -1
		                     );

		goto error;

	}

	lppsStream=lppsStreamBegin;

	lpNextBlock=Dos9_GetNextBlockBegin(lpCh);
	lpSearchBegin=lpCh;

	while ((lpNextToken=Dos9_SearchToken_OutQuotes(lpSearchBegin, "|&"))) {

		if ((lpNextToken >= lpNextBlock)
		    && (lpNextBlock != NULL)) {

			lpSearchBegin=Dos9_GetNextBlockEnd(lpNextBlock);

			if (lpSearchBegin == NULL) {

				Dos9_ShowErrorMessage(DOS9_MALFORMED_BLOCKS, lpCh, FALSE);

				goto error;

			}

			lpNextBlock=Dos9_GetNextBlockBeginEx(lpSearchBegin + 1, 1);

			continue;
		}

		if (*lpNextToken == '&'
            && (lpNextToken >= lpesLine->str +1)
            && (*(lpNextToken - 1) == '>')) {

            lpSearchBegin = lpNextToken + 1;
            continue;

        }

		cChar=*lpNextToken;
		*lpNextToken='\0';
		lpNextToken++;

		/* copy the data into the parsed stream structure */
		Dos9_EsCpy(lppsStream->lpCmdLine, lpCh);
		lppsStream->cNodeType=cNodeType;

		if (!(lppsStream=Dos9_AllocParsedLine(lppsStream))) {

			Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
			                      __FILE__ "/Dos9_ParseOperators()",
			                      -1
			                     );

			goto error;

		}

		switch (cChar) {

		case '|':

			if (*lpNextToken=='|') {

				lpNextToken++;
				cNodeType=PARSED_STREAM_NODE_NOT;

			} else {

				cNodeType=PARSED_STREAM_NODE_PIPE;

			}

			break;

		case '&':
			if (*lpNextToken=='&') {

				lpNextToken++;
				cNodeType=PARSED_STREAM_NODE_YES;

			} else {

				cNodeType=PARSED_STREAM_NODE_NONE;

			}

		}

		lpCh=lpNextToken;
		lpSearchBegin=lpCh;
	}

	Dos9_EsCpy(lppsStream->lpCmdLine, lpCh);
	lppsStream->cNodeType=cNodeType;

	return lppsStreamBegin;

error:

	Dos9_FreeParsedLine(lppsStreamBegin);
	return NULL;

}



PARSED_LINE* Dos9_AllocParsedLine(PARSED_LINE* lppsStream)
{

	PARSED_LINE* lppsNewElement;

	if ((lppsNewElement=(PARSED_LINE*)malloc(sizeof(PARSED_LINE)))) {

		if (lppsStream)
			lppsStream->lppsNode=lppsNewElement;

		lppsNewElement->lppsNode = NULL;
		lppsNewElement->lpCmdLine = Dos9_EsInit();
		lppsNewElement->sStream = NULL;

		return lppsNewElement;
	}

	return NULL;

}

PARSED_STREAM* Dos9_AllocParsedStream(void)
{
	PARSED_STREAM* lppssStream;

	if ((lppssStream = malloc(sizeof(PARSED_STREAM)))) {

		lppssStream->cOutputMode=0;
        lppssStream->cErrorMode=0;
        lppssStream->cRedir=0;

		lppssStream->lpInputFile=NULL;
		lppssStream->lpOutputFile=NULL;
		lppssStream->lpErrorFile=NULL;

		return lppssStream;
	}

	return NULL;
}

void Dos9_FreeParsedStream(PARSED_STREAM* stream)
{
    if (!stream)
        return;

    if (stream->lpInputFile)
        free(stream->lpInputFile);

    if (stream->lpOutputFile)
        free(stream->lpOutputFile);

	if (stream->lpErrorFile)
        free(stream->lpErrorFile);

	free(stream);
}

void Dos9_FreeParsedLine(PARSED_LINE* line)
{
	PARSED_LINE* last=NULL;

    while (line) {

        if (last)
            free(last);

        last = line;

        if (line->sStream)
            Dos9_FreeParsedStream(line->sStream);

        Dos9_EsFree(line->lpCmdLine);

        line = line->lppsNode;

    }

	if (last)
        free(last);
}
