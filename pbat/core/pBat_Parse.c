/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#include <libpBat.h>

#include "pBat_Core.h"
#include "../errors/pBat_Errors.h"

PARSED_LINE* pBat_ParseLine(ESTR* lpesLine)
{
	PARSED_LINE *item, *begin;

	if (!(begin=pBat_ParseOperators(lpesLine)))
		return NULL;

	item = begin;

    while (item) {

        if (!(item->sStream = pBat_ParseOutput(item->lpCmdLine))) {

            pBat_FreeParsedLine(begin);
            return NULL;

        }

        item = item->lppsNode;

    }

	return begin;
}

PARSED_STREAM* pBat_ParseOutput(ESTR* lpesLine)
{

	char *lpCh=pBat_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char lpCorrect[]="1";
	char cChar;

	ESTR *lpesFinal=pBat_EsInit_Cached(TAG_PARSE_OUTPUT_FINAL),
	      *lpesParam=pBat_EsInit_Cached(TAG_PARSE_OUTPUT_PARAM);

	PARSED_STREAM* lppssStart;

	if (!(lppssStart=pBat_AllocParsedStream())) {

		pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
		                      __FILE__ "/ParseOutput()",
		                      -1);
		goto error;

	}

	lpSearchBegin=lpCh;
	lpNextBlock=pBat_GetNextBlockBegin(lpCh);

	while ((lpNextToken=pBat_SearchToken_OutQuotes(lpSearchBegin, "12<>"))) {

		if ((lpNextToken >= lpNextBlock)
		    && (lpNextBlock!=NULL)) {

			lpSearchBegin=pBat_GetNextBlockEnd(lpNextBlock);

            if (lpSearchBegin==NULL) {

				pBat_FreeParsedStream(lppssStart);

				pBat_ShowErrorMessage(PBAT_MALFORMED_BLOCKS, lpCh, FALSE);


				goto error;

			}

			lpNextBlock=pBat_GetNextBlockBeginEx(lpSearchBegin+1, 1);

			continue;

		}

		cChar=*lpNextToken;

		*lpNextToken='\0';

		pBat_EsCat(lpesFinal, lpCh);

		lpNextToken++;

		switch(cChar) {

		case '2':
			/* test wether this is the beginning of the token
			   '2>&1'
			*/

			if (!strncmp(lpNextToken, ">&1", 3)) {

				if (lppssStart->cRedir) {

                    pBat_FreeParsedStream(lppssStart);

                    pBat_ShowErrorMessage(PBAT_ALREADY_REDIRECTED, "1>&2", FALSE);

                    goto error;

				}

				lppssStart->cRedir =  PARSED_STREAM_STDERR2STDOUT;
				lpCh=lpNextToken+3;

				break;

			}

		case '1':

            if (!strncmp(lpNextToken, ">&2", 3)) {

				if (lppssStart->cRedir) {

                    pBat_FreeParsedStream(lppssStart);

                    pBat_ShowErrorMessage(PBAT_ALREADY_REDIRECTED, "1>&2", FALSE);

                    goto error;

				}

				lppssStart->cRedir = PARSED_STREAM_STDOUT2STDERR;

				lpCh=lpNextToken+3;

				break;

			}

			if (*lpNextToken!='>') {

				lpCorrect[0]=cChar;

				pBat_EsCat(lpesFinal, lpCorrect);

				lpCh=lpNextToken;

				break;

			}

			lpNextToken++;

		case '>' :
			/* this is ouput */

			if ((cChar != '2' && lppssStart->lpOutputFile)
                || (cChar == '2' && lppssStart->lpErrorFile)) {

				pBat_FreeParsedStream(lppssStart);

				pBat_ShowErrorMessage(PBAT_ALREADY_REDIRECTED, lpNextToken, FALSE);

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

			if (!(lpCh=pBat_GetNextParameterEsD(lpNextToken, lpesParam,
                                                    PBAT_DELIMITERS "&|><)"))) {

				pBat_FreeParsedStream(lppssStart);

				pBat_ShowErrorMessage(PBAT_INVALID_REDIRECTION, lpNextToken, FALSE);

				goto error;

			}

			if ((cChar != '2' && !(lppssStart->lpOutputFile =
                                pBat_FullPathDup(pBat_EsToChar(lpesParam))))
                || (cChar == '2' && !(lppssStart->lpErrorFile =
                                pBat_FullPathDup(pBat_EsToChar(lpesParam))))) {

				pBat_FreeParsedStream(lppssStart);

				pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
				                      __FILE__ "/ParseOutput()",
				                      -1);

				goto error;

			}


			break;

		case '<' :

			if (lppssStart->lpInputFile) {

				pBat_FreeParsedStream(lppssStart);

				pBat_ShowErrorMessage(PBAT_ALREADY_REDIRECTED, lpNextToken, FALSE);

				goto error;


			}

			if (!(lpCh=pBat_GetNextParameterEsD(lpNextToken, lpesParam,
                                                 PBAT_DELIMITERS "&|><)"))) {

				pBat_FreeParsedStream(lppssStart);

				pBat_ShowErrorMessage(PBAT_INVALID_REDIRECTION, lpNextToken, FALSE);

				goto error;

			}

			if (!(lppssStart->lpInputFile=pBat_FullPathDup(pBat_EsToChar(lpesParam)))) {

				pBat_FreeParsedStream(lppssStart);

				pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
				                      __FILE__ "/ParseOutput()",
				                      -1);

				goto error;

			}

		}

		lpSearchBegin=lpCh;

	}

	pBat_EsCat(lpesFinal, lpCh);
	pBat_EsCpyE(lpesLine, lpesFinal);


	pBat_EsFree_Cached(lpesFinal);
	pBat_EsFree_Cached(lpesParam);

	return lppssStart;

error:
	/* if some fail happened, free memory. However, the
	   origin string will be useless */
	pBat_EsFree_Cached(lpesFinal);
	pBat_EsFree_Cached(lpesParam);
	return NULL;

}

PARSED_LINE*       pBat_ParseOperators(ESTR* lpesLine)
{
	PARSED_LINE *lppsStream=NULL,
					*lppsStreamBegin=NULL;

	char *lpCh=pBat_EsToChar(lpesLine),
	      *lpNextToken,
	      *lpSearchBegin,
	      *lpNextBlock;

	char cChar,
	     cNodeType=PARSED_STREAM_NODE_NONE;

	if (!(lppsStreamBegin=pBat_AllocParsedLine(NULL))) {

		pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
		                      __FILE__ "/pBat_ParseOperators()",
		                      -1
		                     );

		goto error;

	}

	lppsStream=lppsStreamBegin;

	lpNextBlock=pBat_GetNextBlockBegin(lpCh);
	lpSearchBegin=lpCh;

	while ((lpNextToken=pBat_SearchToken_OutQuotes(lpSearchBegin, "|&"))) {

		if ((lpNextToken >= lpNextBlock)
		    && (lpNextBlock != NULL)) {

			lpSearchBegin=pBat_GetNextBlockEnd(lpNextBlock);

			if (lpSearchBegin == NULL) {

				pBat_ShowErrorMessage(PBAT_MALFORMED_BLOCKS, lpCh, FALSE);

				goto error;

			}

			lpNextBlock=pBat_GetNextBlockBeginEx(lpSearchBegin + 1, 1);

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
		pBat_EsCpy(lppsStream->lpCmdLine, lpCh);
		lppsStream->cNodeType=cNodeType;

		if (!(lppsStream=pBat_AllocParsedLine(lppsStream))) {

			pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION | PBAT_PRINT_C_ERROR,
			                      __FILE__ "/pBat_ParseOperators()",
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

	pBat_EsCpy(lppsStream->lpCmdLine, lpCh);
	lppsStream->cNodeType=cNodeType;

	return lppsStreamBegin;

error:

	pBat_FreeParsedLine(lppsStreamBegin);
	return NULL;

}



PARSED_LINE* pBat_AllocParsedLine(PARSED_LINE* lppsStream)
{

	PARSED_LINE* lppsNewElement;

	if ((lppsNewElement=(PARSED_LINE*)malloc(sizeof(PARSED_LINE)))) {

		if (lppsStream)
			lppsStream->lppsNode=lppsNewElement;

		lppsNewElement->lppsNode = NULL;
		lppsNewElement->lpCmdLine = pBat_EsInit(); /* caching is not guaranteed */
		lppsNewElement->sStream = NULL;

		return lppsNewElement;
	}

	return NULL;

}

PARSED_STREAM* pBat_AllocParsedStream(void)
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

void pBat_FreeParsedStream(PARSED_STREAM* stream)
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

void pBat_FreeParsedLine(PARSED_LINE* line)
{
	PARSED_LINE* last=NULL;

    while (line) {

        if (last)
            free(last);

        last = line;

        if (line->sStream)
            pBat_FreeParsedStream(line->sStream);

        pBat_EsFree(line->lpCmdLine);

        line = line->lppsNode;

    }

	if (last)
        free(last);
}

PARSED_STREAM* pBat_DuplicateParsedStream(PARSED_STREAM* stream)
{
    PARSED_STREAM* ret;

    if (!(ret = malloc(sizeof(PARSED_STREAM))))
        goto err;

    ret->cErrorMode = stream->cErrorMode;
    ret->cRedir = stream->cRedir;
    ret->cOutputMode = stream->cOutputMode;

    ret->lpErrorFile = NULL;
    ret->lpInputFile = NULL;
    ret->lpOutputFile = NULL;

    if ((stream->lpErrorFile
            && !(ret->lpErrorFile = strdup(stream->lpErrorFile)))
        || (stream->lpOutputFile
            && !(ret->lpOutputFile = strdup(stream->lpOutputFile)))
        || (stream->lpInputFile
            && !(ret->lpInputFile = strdup(stream->lpInputFile))))
        goto err;

    return ret;

err:
    pBat_FreeParsedStream(ret);

    return NULL;
}
