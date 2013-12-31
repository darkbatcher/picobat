#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef WIN32

    #include "libintl.h"
    #include "iconv.h"

#else

    #include <libintl.h>
    #include <iconv.h>

#endif

#include "hlp_load.h"
#include "hlp_enc.h"

static int iLine=0;
static char lpFile[FILENAME_MAX];

int Hlp_LoadManPage(char* lpPageName, char* lpPageTitle, size_t iSize, HLPPAGE** lpPageRecieve, HLPLINKS** lpLinksRecieve)
{
    FILE * pFile;
    ESTR* lpEsTmp=Dos9_EsInit();
    ESTR* lpEsConv=Dos9_EsInit();

    HLPPAGE* lpLine=NULL;


    HLPLINKS* lpLinks=NULL;

    int iCurrentType=0;

    char* lpToken;
    char* lpCarriageReturn;
    char lpEnc[15]="ASCII";

    snprintf(lpFile, sizeof(lpFile), "%s", lpPageName);

    /* if the pointers are not yet initialized */
    *lpPageRecieve=NULL;
    *lpLinksRecieve=NULL;

    /* the iconv context */
    iconv_t cd;

    Dos9_GetConsoleEncoding(lpEnc, sizeof(lpEnc));

    cd = iconv_open(lpEnc, "UTF-8");

    if (cd == (iconv_t)(-1)) {

        HANDLE_ERROR(gettext("HLP :: %s : Unable to convert file charset : %s"), lpFile, strerror(errno));

    }

    if ( !(pFile=fopen(lpFile, "r")) ) {

        Dos9_EsFree(lpEsTmp);
        HANDLE_ERROR(gettext("HLP :: %s : Unable to load man page : %s"), lpFile, strerror(errno));

    }

    iLine=0;

    while ( !Dos9_EsGet(lpEsTmp, pFile) ) {

        iLine++;

        lpToken=Dos9_EsToChar(lpEsTmp);

        if (iCurrentType==0 && *lpToken=='\x01') {
            lpToken++;
            iCurrentType=1;
        }

        if (iCurrentType==1 && *lpToken=='\x02') {
            lpToken++;
            iCurrentType=2;
        }

        if (iCurrentType==1) {

            /* wiping the carriot return from the output */
            if ( (lpCarriageReturn=strchr(lpToken, '\n')) ) {
                *lpCarriageReturn='\0';
            }

            /* this is what to done if the line is a part of the header */
            if (!strncmp(lpToken, "title:", 6)) {

                /* if the line is containing the title */
                lpToken+=6;

                strncpy(lpPageTitle, lpToken, iSize);

                lpPageTitle[iSize-1] = '\0';
                /* the function strncpy() does not garantee that a NUL character will be inserted at
                   the end of the recieving buffer when the string to copy is longer than the buffer
                   to which the string will be copied
                */

            } else if (!strncmp(lpToken, "link[", 5)){
                /* this is a normal link line */

                lpToken+=5;

                if (!lpLinks) {

                    lpLinks=Hlp_AllocLinkElement();
                    *lpLinksRecieve=lpLinks;

                } else {

                    lpLinks->lpNextLink=Hlp_AllocLinkElement();
                    lpLinks=lpLinks->lpNextLink;

                }

                lpLinks->iLinkNb=strtol(lpToken, &lpToken, 0); /* we get the number of the link */

                if (!strncmp(lpToken, "]:", 2)) {

                    lpToken+=2;

                    strncpy(lpLinks->lpTarget, lpToken, FILENAME_MAX);

                    lpLinks->lpTarget[FILENAME_MAX-1]='\0';

                } else {

                    goto loadInvalidHeader; /* invalid header */

                }

            } else {

                loadInvalidHeader:

                    HANDLE_ERROR(gettext("HLP :: %s : %d : Invalid header in file \n"), lpFile, iLine);

            }

        } else if (iCurrentType==2) {

            /* this is what to do if the line is a part of the content */

            /* first we allocate a new line */
            if (!lpLine) {

                lpLine=Hlp_AllocPage();
                *lpPageRecieve=lpLine;

            } else {

                lpLine->lpNextLine=Hlp_AllocPage();
                lpLine->lpNextLine->lpPreviousLine=lpLine;
                lpLine=lpLine->lpNextLine;

            }

            /* this is the parsing time */
            if (Hlp_StringConvert(cd, lpEsConv, lpToken)) {

                HANDLE_ERROR(gettext("HLP :: %s : %d : Error, failed to translate strings"), lpFile ,iLine);

            } else {

                lpLine->lpLineContent=Hlp_ParseLine(Dos9_EsToChar(lpEsConv));

            }

        }

    }

    iconv_close(cd);

    Dos9_EsFree(lpEsTmp);
    Dos9_EsFree(lpEsConv);
    fclose(pFile);

    return 0;
}

HLPLINKS* Hlp_AllocLinkElement(void)
{
    HLPLINKS* lpReturn;

    if ( (lpReturn=malloc(sizeof(HLPLINKS)) )) {

        lpReturn->iLinkNb=0;
        lpReturn->lpNextLink=NULL;

        if ( !(lpReturn->lpTarget=malloc(FILENAME_MAX)) ) {

            goto allocLinkError;

        }

        return lpReturn;
    }


    allocLinkError:

        perror(gettext("HLP :: Unable to allocate memory for ``HLPLINKS''"));

        exit(-1);

}

void      Hlp_FreeLinkElement(HLPLINKS* lpLink)
{
    if (lpLink->lpTarget) free(lpLink->lpTarget);
    free(lpLink);
}

HLPELEMENT* Hlp_AllocHlpElement(void)
{
    HLPELEMENT* lpReturn;

    if ( (lpReturn=malloc(sizeof(HLPELEMENT))) ) {

        lpReturn->iType=HLP_ELEMENT_TEXT;
        lpReturn->lpContent=NULL;
        lpReturn->lpNext=NULL;

        return lpReturn;

    }

        HANDLE_ERROR(gettext("HLP :: Unable to allocate memory for ``HLPELEMENT'' : %s"), strerror(errno));
        exit(-1);

}

void        Hlp_FreeHlpElement(HLPELEMENT* lpElement)
{
    if (lpElement->lpContent) free(lpElement->lpContent);
    free(lpElement);
}


HLPELEMENT* Hlp_ParseLine(char* lpLine)
{
    /* this function parses each line */

    char* lpToken;
    int   iBlockType=0;

    HLPELEMENT* lpHlp;
    HLPELEMENT* lpHlpReturn;

    lpHlp=Hlp_AllocHlpElement();
    lpHlpReturn=lpHlp;

    while ( (lpToken=strpbrk(lpLine, "\x11\x12\x13\x14")) ) {

        switch (*lpToken) {

            case '\x11' :

                iBlockType=HLP_ELEMENT_HEADING;
                break;

            case '\x12' :

                iBlockType=HLP_ELEMENT_CODE;
                break;

            case '\x13' :

                iBlockType=HLP_ELEMENT_EMPHASIS;
                break;

            case '\x14' :

                iBlockType=HLP_ELEMENT_LINK;

        }

        *lpToken='\0';

        if ( !(lpHlp->lpContent=strdup(lpLine)) ) goto lineParseError;

        lpHlp->lpNext=Hlp_AllocHlpElement();
        lpHlp=lpHlp->lpNext;

        lpLine=lpToken+1;

        if ( (lpToken=strchr(lpLine, '\x10')) ) {

            *lpToken='\0';

            lpHlp->iType=iBlockType;

            if ( !(lpHlp->lpContent=strdup(lpLine)) ) goto lineParseError;

            lpHlp->lpNext=Hlp_AllocHlpElement();
            lpHlp=lpHlp->lpNext;

            lpLine=lpToken+1;

        } else {

            HANDLE_ERROR(gettext("HLP :: %s : %d : Invalid text section (Can't find ``DLE'' character)\n"
								 "%s\n"), lpFile, iLine);

        }

    }

    if ( !(lpHlp->lpContent=strdup(lpLine)) ) goto lineParseError;

    return lpHlpReturn;

    lineParseError:

        HANDLE_ERROR(gettext("HLP :: Unable to allocate memory for ``CHAR'' : %s"), strerror(errno));

}

void        Hlp_FreeLine(HLPELEMENT* lpLine)
{

    HLPELEMENT* lpLineTmp;

    while (lpLine) {

        lpLineTmp=lpLine->lpNext;

        Hlp_FreeHlpElement(lpLine);

        lpLine=lpLineTmp;

    }
}

HLPPAGE* Hlp_AllocPage(void)
{
    HLPPAGE* lpReturn;

    if ( (lpReturn=malloc(sizeof(HLPPAGE))) ) {

        lpReturn->lpLineContent=NULL;
        lpReturn->lpNextLine=NULL;
        lpReturn->lpPreviousLine=NULL;

        return lpReturn;
    }

    HANDLE_ERROR(gettext("HLP :: Unable to allocate memory for ``HLPPAGE'' : %s"), strerror(errno));

}

void     Hlp_FreePage(HLPPAGE* lpPage)
{
    if (lpPage->lpLineContent) Hlp_FreeLine(lpPage->lpLineContent);
    free(lpPage);
}

void Hlp_FreeManPage(HLPPAGE* lpPage)
{

    HLPPAGE* lpPageTmp;
    HLPPAGE* lpPrevious=lpPage;
    HLPPAGE* lpNext=lpPage->lpNextLine;

    /* on vide ce qui se trouve au dessus de la ligne */

    while (lpPrevious) {

        lpPageTmp=lpPrevious->lpPreviousLine;

        Hlp_FreePage(lpPrevious);

        lpPrevious=lpPageTmp;

    }

    /*on vide ce qui se trouve en dessous de la ligne */

    while (lpNext) {

        lpPageTmp=lpNext->lpNextLine;

        Hlp_FreePage(lpNext);

        lpNext=lpPageTmp;

    }

}

void Hlp_FreeManPageLinks(HLPLINKS* lpLinks)
{
    HLPLINKS* lpLinksTmp;

    while (lpLinks) {

        lpLinksTmp=lpLinks->lpNextLink;

        Hlp_FreeLinkElement(lpLinks);

        lpLinks=lpLinksTmp;

    }
}
