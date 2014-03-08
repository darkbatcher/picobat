#include <stdio.h>
#include <stdlib.h>

#include "Dos9_Core.h"


#define DOS9_DBG_MODE

#include "Dos9_Debug.h"

int Dos9_GetLine(ESTR* lpesLine, INPUT_FILE* pIn)
{
    FILE* pFile;
    ESTR* lpesTmp=Dos9_EsInit();

    char bCorrectBlocks=FALSE;
    char* lpCh;

    if (pIn->lpFileName == NULL) {

        pFile=stdin;

    } else if (!(pFile=fopen(pIn->lpFileName, "r"))) {

        Dos9_ShowErrorMessage(DOS9_FILE_ERROR, pIn->lpFileName, FALSE);
        goto error;

    } else {

        fseek(pFile, pIn->iPos, SEEK_SET);

    }

    *(Dos9_EsToChar(lpesLine))='\0';

    while (!(Dos9_EsGet(lpesTmp, pFile))) {

        lpCh=Dos9_SkipAllBlanks(Dos9_EsToChar(lpesTmp));

        /* split comments label and void lines from input */
        if (*lpCh!=':' && *lpCh!='\n')
            Dos9_EsCatE(lpesLine, lpesTmp);

        if (Dos9_CheckBlocks(lpesLine) == TRUE) {

            bCorrectBlocks=TRUE;
            break;

        }
    }

    if (pIn->lpFileName) {

        pIn->iPos=ftell(pFile);
        pIn->bEof=feof(pFile);

        fclose(pFile);

    }

    if (!bCorrectBlocks) {

        Dos9_ShowErrorMessage(DOS9_NONCLOSED_BLOCKS, NULL, FALSE);
        goto error;

    }

    Dos9_EsFree(lpesTmp);
    Dos9_RmTrailingNl(Dos9_EsToChar(lpesLine));

    return ( *(Dos9_SkipAllBlanks(Dos9_EsToChar(lpesLine))) == '\0' )
           ? 1 : 0;

    error:
        Dos9_EsFree(lpesTmp);
        return -1;

}

int Dos9_CheckBlocks(ESTR* lpesLine)
{
    char *lpBlock,
         *lpCh=Dos9_EsToChar(lpesLine);


    if (!(lpBlock=Dos9_GetNextBlockBegin(lpCh))) {

        /* There's no block, thus all is fine */

        return TRUE;

    }

    /* lpBlockBegin we have is the highest level block
       that is avaliable, just perform some check for it */

    do {

        if (!(lpCh=Dos9_GetBlockEnd(lpBlock)))
            return FALSE;

    } while ((lpBlock=Dos9_GetNextBlockBegin(lpCh)));

    return TRUE;

}

void Dos9_RmTrailingNl(char* lpLine)
{
	char cLastChar=0;

	while (*lpLine) {

		cLastChar=*(lpLine++);

	}

	if (cLastChar=='\n')
		*(lpLine-1)='\0';

}
