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

PARSED_STREAM_START* Dos9_ParseLine(ESTR* lpesLine)
{
	PARSED_STREAM_START* lppssReturn; /* a pointer to the structure to be
                                         returned */

	PARSED_STREAM* lppsStream;

	if (!(lppssReturn=Dos9_ParseOutput(lpesLine)))
		return NULL;

	if (!(lppsStream=Dos9_ParseOperators(lpesLine))) {

		Dos9_FreeLine(lppssReturn);
		return NULL;

	}

	lppssReturn->lppsStream=lppsStream;

	return lppssReturn;
}

PARSED_STREAM_START* Dos9_ParseOutput(ESTR* lpesLine)
{

	char *lpCh=Dos9_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char lpCorrect[]="1";
	char cChar;

	ESTR *lpesFinal=Dos9_EsInit(),
	      *lpesParam=Dos9_EsInit();

	PARSED_STREAM_START* lppssStart;

	if (!(lppssStart=Dos9_AllocParsedStreamStart())) {

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

				Dos9_FreeLine(lppssStart);

				Dos9_ShowErrorMessage(DOS9_MALFORMED_BLOCKS, lpCh, FALSE);


				goto error;

			}
            
			lpNextBlock=Dos9_GetNextBlockBeginEx(lpSearchBegin, 1);

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

				/* redirect stderr in stdout */
				lppssStart->cOutputMode|=(PARSED_STREAM_START_MODE_ERROR
				                          | PARSED_STREAM_START_MODE_OUT );

				lpCh=lpNextToken+3;

				break;

			}

		case '1':

			if (*lpNextToken!='>') {

				lpCorrect[0]=cChar;

				Dos9_EsCat(lpesFinal, lpCorrect);

				lpCh=lpNextToken;

				break;

			}

			lpNextToken++;

		case '>' :
			/* this is ouput */

            fprintf(stderr, "found > \"%s\".\n", lpNextToken); 
			if (lppssStart->lpOutputFile) {

				Dos9_FreeLine(lppssStart);

				Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED, lpNextToken, FALSE);

				goto error;


			}

			if (*lpNextToken!='>') {

				lppssStart->cOutputMode|=PARSED_STREAM_START_MODE_TRUNCATE;

			} else {

				lpNextToken++;

			}

			if (!(lpCh=Dos9_GetNextParameterEs(lpNextToken, lpesParam))) {

				Dos9_FreeLine(lppssStart);

				Dos9_ShowErrorMessage(DOS9_INVALID_REDIRECTION, lpNextToken, FALSE);

				goto error;

			}

			/* determine redirection type */
			if (cChar=='2') {

				lppssStart->cOutputMode|=
				    (lppssStart->cOutputMode | PARSED_STREAM_START_MODE_ERROR)
				    & ~PARSED_STREAM_START_MODE_OUT;

			} else {

				lppssStart->cOutputMode|=PARSED_STREAM_START_MODE_OUT;

			}

			if (!(lppssStart->lpOutputFile=strdup(Dos9_EsToChar(lpesParam)))) {

				Dos9_FreeLine(lppssStart);

				Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION | DOS9_PRINT_C_ERROR,
				                      __FILE__ "/ParseOutput()",
				                      -1);

				goto error;

			}


			break;

		case '<' :
            fprintf(stderr, "found > \"%s\".\n", lpNextToken); 
			/* this is input */

			if (lppssStart->lpInputFile) {

				Dos9_FreeLine(lppssStart);

				Dos9_ShowErrorMessage(DOS9_ALREADY_REDIRECTED, lpNextToken, FALSE);

				goto error;


			}

			if (!(lpCh=Dos9_GetNextParameterEs(lpNextToken, lpesParam))) {

				Dos9_FreeParsedStreamStart(lppssStart);

				Dos9_ShowErrorMessage(DOS9_INVALID_REDIRECTION, lpNextToken, FALSE);

				goto error;

			}

			if (!(lppssStart->lpInputFile=strdup(Dos9_EsToChar(lpesParam)))) {

				Dos9_FreeLine(lppssStart);

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

PARSED_STREAM*       Dos9_ParseOperators(ESTR* lpesLine)
{
	PARSED_STREAM *lppsStream=NULL,
					*lppsStreamBegin=NULL;

	char *lpCh=Dos9_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char cChar,
	     cNodeType=PARSED_STREAM_NODE_NONE;

	if (!(lppsStreamBegin=Dos9_AllocParsedStream(NULL))) {

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

			lpNextBlock=Dos9_GetNextBlockBeginEx(lpSearchBegin, 1);

			continue;
		}

		cChar=*lpNextToken;
		*lpNextToken='\0';
		lpNextToken++;

		/* copy the data into the parsed stream structure */
		Dos9_EsCpy(lppsStream->lpCmdLine, lpCh);
		lppsStream->cNodeType=cNodeType;

		if (!(lppsStream=Dos9_AllocParsedStream(lppsStream))) {

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

	Dos9_FreeParsedStream(lppsStreamBegin);
	return NULL;

}



PARSED_STREAM* Dos9_AllocParsedStream(PARSED_STREAM* lppsStream)
{

	PARSED_STREAM* lppsNewElement;

	if ((lppsNewElement=(PARSED_STREAM*)malloc(sizeof(PARSED_STREAM)))) {
		if (lppsStream)
			lppsStream->lppsNode=lppsNewElement;

		lppsNewElement->lppsNode=NULL;
		lppsNewElement->lpCmdLine=Dos9_EsInit();

		return lppsNewElement;
	}

	return NULL;

}

PARSED_STREAM_START* Dos9_AllocParsedStreamStart(void)
{
	PARSED_STREAM_START* lppssStreamStart;

	if ((lppssStreamStart=malloc(sizeof(PARSED_STREAM_START)))) {

		lppssStreamStart->cOutputMode=0;

		lppssStreamStart->lppsStream=NULL;
		lppssStreamStart->lpInputFile=NULL;
		lppssStreamStart->lpOutputFile=NULL;

		return lppssStreamStart;
	}

	return NULL;
}

void Dos9_FreeParsedStreamStart(PARSED_STREAM_START* lppssStart)
{
	free(lppssStart->lpInputFile);
	free(lppssStart->lpOutputFile);

	Dos9_FreeParsedStream(lppssStart->lppsStream);

	free(lppssStart);
}

void Dos9_FreeParsedStream(PARSED_STREAM* lppsStream)
{
	PARSED_STREAM* lppsLast=NULL;

	for(; lppsStream; lppsStream=lppsStream->lppsNode) {

		if (lppsLast)
			free(lppsLast);

		lppsLast=lppsStream;
		Dos9_EsFree(lppsStream->lpCmdLine);

	}

	if (lppsLast) free(lppsLast);
}



