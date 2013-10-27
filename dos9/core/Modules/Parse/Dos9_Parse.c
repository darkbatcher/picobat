#include "../../Dos9_Core.h"
#include "../../../errors/Dos9_Errors.h"



extern COLOR colColor;

int MODULE_MAIN Dos9_ParseModule(int iMsg, void* param1, void* param2)
{
   static ESTR *lpesCommandLine, *lpesLine;
   ESTR* lpTmp;
   char* const lpCurrentDir=Dos9_GetCurrentDir();
   PARSED_STREAM* lppsParsedStream;
   PARSED_STREAM_START* lppssStreamStart;
   int iLastParentNb;
   char* lpLastCr=NULL;

   switch(iMsg) {

       case MODULE_PARSE_INIT:

            lpesCommandLine=Dos9_EsInit();
            lpesLine=Dos9_EsInit();
            lppsParsedStream=NULL;
            return 0;

       case MODULE_PARSE_READ_LINE_PARSE:

            Dos9_EsCpy(lpesCommandLine, "");
            *Dos9_EsToChar(lpesLine)='\0';
            iLastParentNb=0;
            DEBUG("Entering parsing");
            if (bEchoOn && !param2) {

                DEBUG("Displaying current dir for input");
                Dos9_SetConsoleTextColor(DOS9_GET_BACKGROUND(colColor) | DOS9_FOREGROUND_IGREEN);
                printf("\nDOS9" );
                Dos9_SetConsoleTextColor(colColor);
                printf(" %s>", lpCurrentDir);

            }

            if ((char*)param1!=NULL) {
                Dos9_EsCpy(lpesCommandLine, (char*)param1);
                DEBUG("Copy the content of the line");
            } else {

                do {
                    Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_GETLINE, lpesLine,NULL);
                    DEBUG("Reading line from the file");
                    DEBUG(Dos9_EsToChar(lpesLine));

                    Dos9_ReplaceVars(lpesLine);
                    Dos9_EsCatE(lpesCommandLine, lpesLine);

                } while ((iLastParentNb=Dos9_CountParenthese(Dos9_EsToChar(lpesLine),iLastParentNb)) && !Dos9_SendMessage(DOS9_READ_MODULE, MODULE_READ_ISEOF, NULL, NULL));

            }

            DEBUG(Dos9_EsToChar(lpesCommandLine));

            if ((*Dos9_EsToChar(lpesCommandLine)=='\0'  || *Dos9_EsToChar(lpesCommandLine)=='\n' || *Dos9_EsToChar(lpesCommandLine)=='\r')) {
                return ((int)NULL);
            }

            if ((lpLastCr=strrchr(Dos9_EsToChar(lpesCommandLine), '\n'))) *lpLastCr='\0';

            if ((lpLastCr=strrchr(Dos9_EsToChar(lpesCommandLine), '\r'))) *lpLastCr='\0';

            if (bEchoOn && param2 && *(Dos9_EsToChar(lpesCommandLine))!='@') {

              DEBUG("Displaying cd for files");
              printf("\nDOS9  %s>", lpCurrentDir);
              puts(Dos9_EsToChar(lpesCommandLine));

            }

            lppssStreamStart=Dos9_ParseStreamOutput(Dos9_EsToChar(lpesCommandLine));
            DEBUG(Dos9_EsToChar(lpesCommandLine));
            lppssStreamStart->lppsStream=Dos9_ParseStream(Dos9_EsToChar(lpesCommandLine));

            return (int)lppssStreamStart;

        case MODULE_PARSE_PARSE_ESTR:
            lpTmp=(ESTR*)param1;
            lppssStreamStart=Dos9_ParseStreamOutput(Dos9_EsToChar(lpTmp));
            DEBUG(Dos9_EsToChar(lpTmp));
            lppssStreamStart->lppsStream=Dos9_ParseStream(Dos9_EsToChar(lpTmp));
            return (int)lppssStreamStart;

        case MODULE_PARSE_FREE_PARSED_LINE:
            return Dos9_BeginThread((void (*)(void *))Dos9_FreeParsedStreamStart, 0, (void*)((PARSED_STREAM_START*)param1));

        case MODULE_PARSE_NEWLINE:
            //lppsStreamStack=Dos9_PushStreamStack(lppsStreamStack);
            return 0;

        case MODULE_PARSE_ENDLINE:
            lppsStreamStack=Dos9_PopStreamStack(lppsStreamStack);
            return 0;

        case MODULE_PARSE_PARSED_START_EXEC:
            /* this message sets the major redirection level */

            lppssStreamStart=(PARSED_STREAM_START*)param1;
            DEBUG("Running start execution with flag :");
            DEBUG_(lppssStreamStart->cOutputMode);
            DEBUG("The satement returned:");
            DEBUG_(lppssStreamStart->cOutputMode & ~PARSED_STREAM_START_MODE_TRUNCATE);
            if (lppssStreamStart->lpInputFile || lppssStreamStart->lpOutputFile ) {
                lppsStreamStack=Dos9_PushStreamStack(lppsStreamStack);
            }
            if (lppssStreamStart->cOutputMode  && lppssStreamStart->lpOutputFile ) Dos9_OpenOutput(lppsStreamStack, lppssStreamStart->lpOutputFile, lppssStreamStart->cOutputMode & ~PARSED_STREAM_START_MODE_TRUNCATE, lppssStreamStart->cOutputMode & PARSED_STREAM_START_MODE_TRUNCATE);
            if (lppssStreamStart->lpInputFile) Dos9_OpenOutput(lppsStreamStack, lppssStreamStart->lpInputFile, DOS9_STDIN, 0);
            return 0;

        case MODULE_PARSE_PARSED_LINE_EXEC:

            lppsParsedStream=(PARSED_STREAM*)param1;
            lppsStreamStack=Dos9_Pipe(lppsStreamStack);
            switch (lppsParsedStream->cNodeType)
            {
                case PARSED_STREAM_NODE_NONE:
                    /* no condition */
                    return (int)lppsParsedStream->lppsNode;
                case PARSED_STREAM_NODE_PIPE:
                    return (int)lppsParsedStream->lppsNode;
                case PARSED_STREAM_NODE_YES:
                    if (!iErrorLevel) return (int)lppsParsedStream->lppsNode;
                    return (int)NULL;
                case PARSED_STREAM_NODE_NOT:
                    if (iErrorLevel) return (int)lppsParsedStream->lppsNode;
                    return (int)NULL;
            }
            return (int)NULL;

        case MODULE_PARSE_PIPE_OPEN:
            lppsParsedStream=(PARSED_STREAM*)param1;
            if (lppsParsedStream->cNodeType==PARSED_STREAM_NODE_PIPE && lppsParsedStream->lppsNode!=NULL) Dos9_OpenPipe(lppsStreamStack);
            return 0;

        default:
            return -1;
   }
}

int Dos9_CountParenthese(char* lpLine, int iLastNb)
{
    int iLastEscape=0;
    for (;*lpLine;lpLine++) {

        if (iLastEscape) {
            iLastEscape=0;
            continue;
        }

        switch (*lpLine) {
            case '(':
                iLastNb++;
                break;

            case ')':
                if (iLastNb>0) iLastNb--;
                break;

            case '^':
                iLastEscape=1;
            default:;
        }
    }
    return iLastNb;
}

PARSED_STREAM* Dos9_ParseStream(char* lpLine)
{
    char *ptrToken=lpLine;
    char cLastEscape=0;
    int iNbParenthese=0;
    PARSED_STREAM* lppsFirstParsedLine=NULL, *lppsParsedLine;
    lppsFirstParsedLine=lppsParsedLine=Dos9_AllocParsedStream(NULL);
    for (;*lpLine;lpLine++) {
        if (cLastEscape) {
            cLastEscape=0;
            continue;
        }

        if (iNbParenthese) {
            switch(*lpLine) {
                case '^':
                    cLastEscape=1;
                    continue;
                case '(':
                    iNbParenthese++;
                    continue;
                case ')':
                    iNbParenthese--;
                    continue;
                default: continue;
            }
        }

        switch(*lpLine) {

            case '(':

                iNbParenthese++;
                continue;

            case '^':

                cLastEscape=1;
                continue;

            case '|':

                if (!strncmp(lpLine, "||", 2)) {

                    lppsParsedLine->cNodeType=PARSED_STREAM_NODE_NOT;
                    *lpLine='\0';
                    lpLine++;

                } else {

                    lppsParsedLine->cNodeType=PARSED_STREAM_NODE_PIPE;
                    *lpLine='\0';

                }

                break;

            case '&':

                if (!strncmp(lpLine, "&&", 2)){

                   lppsParsedLine->cNodeType=PARSED_STREAM_NODE_YES;
                   *lpLine='\0';
                   lpLine++;

                } else {

                   lppsParsedLine->cNodeType=PARSED_STREAM_NODE_NONE;
                    *lpLine='\0';

                }

                break;

            default:

                continue;
        }

        Dos9_EsCpy(lppsParsedLine->lpCmdLine, ptrToken);
        Dos9_EsCat(lppsParsedLine->lpCmdLine, " ");
        DEBUG(ptrToken);
        ptrToken=lpLine+1;
        lppsParsedLine=Dos9_AllocParsedStream(lppsParsedLine);

    }

    Dos9_EsCpy(lppsParsedLine->lpCmdLine, ptrToken);
    Dos9_EsCat(lppsParsedLine->lpCmdLine, " ");
    DEBUG(ptrToken);
    return lppsFirstParsedLine;
}

PARSED_STREAM* Dos9_AllocParsedStream(PARSED_STREAM* lppsStream)
{
    PARSED_STREAM* lppsNewElement;
    if ((lppsNewElement=(PARSED_STREAM*)malloc(sizeof(PARSED_STREAM)))) {
        if (lppsStream) lppsStream->lppsNode=lppsNewElement;
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

PARSED_STREAM_START* Dos9_ParseStreamOutput(char *lpLine)
{
    int iParentheseNb=0;
    int iLastEscape=1;
    char* lpLastLine;
    PARSED_STREAM_START* lppssStreamStart=Dos9_AllocParsedStreamStart();
    for (;*lpLine;lpLine++) {
        if (iLastEscape) {
            iLastEscape=0;
            continue;
        }
        if (iParentheseNb==0) {

            switch (*lpLine){
                case '(':
                    iParentheseNb++;
                    break;
                case '^':
                    iLastEscape=1;
                    break;
                case '2':
                    if (!strncmp(lpLine, "2&>1", 4)) {
                        lppssStreamStart->cOutputMode|=PARSED_STREAM_START_MODE_ERROR;
                        Dos9_AdjustString(lpLine, lpLine+4);
                    } else if (!strncmp(lpLine, "2>", 2)) {
                        lppssStreamStart->cOutputMode=PARSED_STREAM_START_MODE_ERROR;
                        lpLastLine=lpLine;
                        lpLine++;
                        goto ChoseOutputMode;
                    }
                    break;
                case '>':
                    lpLastLine=lpLine;
                    lppssStreamStart->cOutputMode=PARSED_STREAM_START_MODE_OUT;
                    ChoseOutputMode:
                    if (!strncmp(lpLine, ">>", 2)) {
                        lpLine+=2;
                    } else {
                        lpLine++;
                        lppssStreamStart->cOutputMode|=PARSED_STREAM_START_MODE_TRUNCATE;
                    }
                    if (lppssStreamStart->lpOutputFile) free(lppssStreamStart->lpOutputFile);
                    lppssStreamStart->lpOutputFile=Dos9_GetPathToken(lpLine, lpLastLine);
                    lpLine=lpLastLine-1;
                    break;
                case '<':
                    lpLastLine=lpLine;
                    lpLine++;
                    if (lppssStreamStart->lpInputFile) free(lppssStreamStart->lpInputFile);
                    lppssStreamStart->lpInputFile=Dos9_GetPathToken(lpLine, lpLastLine);
                    lpLine=lpLastLine-1;
            }

        } else {

            switch (*lpLine){
                case '(':
                    iParentheseNb++;
                    continue;
                case ')':
                    iParentheseNb--;
                    continue;
                case '^':
                    iLastEscape=1;
            }

        }
    }
    return lppssStreamStart;
}

char* Dos9_GetPathToken(char* lpBegin, char* lpAdjust)
{
    char* lpToken;
    char cSeekQuote=0;
    char cLastBegin;
    while (*lpBegin=='\t' || *lpBegin==' ') lpBegin++;

    if (*lpBegin=='"') {
        lpBegin++;
        cSeekQuote=1;
    }

    lpToken=lpBegin;

    while (*lpBegin)
    {
        if (*lpBegin=='"' && cSeekQuote) break;
        if ((*lpBegin==' ' || *lpBegin=='\t' || *lpBegin=='\n')&& !cSeekQuote) break;
        lpBegin++;
    }
    *lpAdjust='\0';
    cLastBegin=*lpBegin;
    *lpBegin='\0';
    lpToken=strdup(lpToken);

    if (cLastBegin) Dos9_AdjustString(lpAdjust, lpBegin+1);

    return lpToken;
}

void Dos9_AdjustString(char* lpBegin, char* lpEnd)
{
    int i;
    for (i=0;lpEnd[i];i++) lpBegin[i]=lpEnd[i];
    lpBegin[i]='\0';
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
    for(;lppsStream;lppsStream=lppsStream->lppsNode) {
        if (lppsLast) free(lppsLast);
        lppsLast=lppsStream;
        Dos9_EsFree(lppsStream->lpCmdLine);
    }
    if (lppsLast) free(lppsLast);
}

