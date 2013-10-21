#include "../Dos9_CmdLib.h"
#include "../../core/Dos9_Core.h"

DOS9_CMDLIB char* Dos9_GetNextParameter(char* lpLine, char* lpResponseBuffer, int iLength)
/* determines wheter a paramater follows the position lpLine.
 *
 * lpLine : A pointer to where to seek a parameter
 * lpResponse : A buffer to store the parameter
 * iLenght : The maximum length to be stored in lpResponse
 *
 * return : the part after the parameter or null if no parameter has been found
 *
 */
 {
     ESTR* lpParameter=Dos9_EsInit();

     lpLine=Dos9_GetNextParameterEs(lpLine, lpParameter);
     strncpy(lpResponseBuffer, Dos9_EsToChar(lpParameter), iLength);
     lpResponseBuffer[iLength-1]='\0';

     Dos9_EsFree(lpParameter);
     return lpLine;
 }

DOS9_CMDLIB char* Dos9_GetNextBlock(char* lpLine, char** lpNextPart)
{
    char* lpBlockBegin;
    char cSave;
    int iParentheseLvl=0;

    while (*lpLine=='\t' || *lpLine==' ') lpLine++;

    if (*lpLine=='(') {

        /* the block might begin here */

        lpLine++;
        lpBlockBegin=lpLine;

        while (*lpLine && (*lpLine!=')' || iParentheseLvl)) {

            /* parenthesis may contain enclosed parenthesis levels,
               skip them */

            if (*lpLine=='(') iParentheseLvl++;
            if (*lpLine==')' && iParentheseLvl>0) iParentheseLvl--;
            lpLine++;

        }

        *lpNextPart=lpLine;

    } else {

        /* The blocks continues until the next carriage return */
        lpBlockBegin=lpLine;
        while (*lpLine && *lpLine!='\n') lpLine++;
        *lpNextPart=lpLine;
    }

    return lpBlockBegin;
}

DOS9_CMDLIB char* Dos9_GetNextBlockEs(char* lpLine, ESTR* lpReturn)
{
    char* lpNext;
    size_t iNbBytes;

    lpLine = Dos9_GetNextBlock(lpLine, &lpNext);


    iNbBytes = lpNext - lpLine + 1;

    Dos9_EsCpyN(lpReturn, lpLine, iNbBytes);


    if (*lpNext != '\0')
        lpNext++;

    return lpNext;

}

DOS9_CMDLIB char* Dos9_GetNextParameterEs(char* lpLine, ESTR* lpReturn)
{
     int iSeekQuote=FALSE;
     int i=0;
     char* lpStartParameter=NULL;

     for (;*lpLine==' ' || *lpLine=='\t';lpLine++);


     if (*lpLine=='"') {
         iSeekQuote=1;
         lpLine++;
     } else if (*lpLine=='\'') {
         iSeekQuote=2;
         lpLine++;
     }

     if (!*lpLine) return NULL;

     lpStartParameter=lpLine;

     for (;*lpLine;lpLine++) {

         i++;

         if (((*lpLine=='"') && (iSeekQuote==1)) || (*lpLine=='\'' && iSeekQuote == 2)) {
             /* if we were seeking a quote */

             if (*(lpLine+1)!='\t' && *(lpLine+1)!=' ') continue; /* if the following character is neither a
                                                                    a space nor a tab, then it continues */
             break;
         }

         if ((*lpLine==' ') & (!iSeekQuote)) break; /* if the programs seeks only a space
                                                       then it breaks */

     }

     Dos9_EsCpyN(lpReturn, lpStartParameter, i);
     Dos9_DelayedExpand(lpReturn, bDelayedExpansion); /* Note : delayed expansion means here all vars expansion
                                                        peformed after the line first buffering from the file,
                                                        thus, it includes also expansion of special-local vars,
                                                        such as %1 */
     return lpLine+1;
}

DOS9_CMDLIB int   Dos9_GetParamArrayEs(char* lpLine, ESTR** lpArray, size_t iLenght)
/* gets command-line argument in an array of extended string

    Note: The function assumes that lpArray array doesn't contain
    non-NULL pointers to extended strings

    This code must be refreshed as it make extensive use of various character
    oriented loop that are already implemented in the previous function. The refreshed
    code must be an iteration of the previous function. This makes things easier to read,
    easier to maintain, easier to upgrade, and less buggy.
*/
{
    int iSeekQuote=FALSE, i=0, j=0;
    char* lpStartPos;

    while (*lpLine==' ' || *lpLine=='\t') lpLine++;
    lpStartPos=lpLine;

    if (*lpLine=='"') {
        iSeekQuote=TRUE;
        i++;
        lpLine++;
    }

    while (*lpLine!='\0' && j<iLenght-1) {
        i++;
        if ((*lpLine==' ') & (!iSeekQuote)) {
            /* if we found a new element */
            lpArray[j]=Dos9_EsInit();
            Dos9_EsCpyN(lpArray[j], lpStartPos, i);
            Dos9_DelayedExpand(lpArray[j], bDelayedExpansion);
            i=0;

            while (*lpLine==' ' || *lpLine=='\t') lpLine++;
            lpStartPos=lpLine;
            if (*lpLine=='"') {
                iSeekQuote=TRUE;
                i++;
                lpLine++;
            }

            j++;

        } else if (*lpLine=='"' && iSeekQuote) {

            if (*(lpLine+1)==' ' || *(lpLine+1)=='\t') {
                lpArray[j]=Dos9_EsInit();
                Dos9_EsCpyN(lpArray[j], lpStartPos, i+1);
                Dos9_DelayedExpand(lpArray[j], bDelayedExpansion);
                i=0;

                lpLine++;
                while (*lpLine==' ' || *lpLine=='\t') lpLine++;
                lpStartPos=lpLine;
                if (*lpLine=='"') {
                    iSeekQuote=TRUE;
                    i++;
                    lpLine++;
                } else {
                    iSeekQuote=FALSE;
                }

                j++;

            } else {

                lpLine++;
                i=0;

            }

        } else {

            lpLine++;

        }
    }

    if (*lpStartPos!='\0') {

        lpArray[j]=Dos9_EsInit();
        Dos9_EsCpyN(lpArray[j], lpStartPos, i+1);
        Dos9_DelayedExpand(lpArray[j], bDelayedExpansion);
        j++;

    }

    lpArray[j]=NULL;
    return 0;
}

DOS9_CMDLIB char* Dos9_GetEndOfLine(char* lpLine, ESTR* lpReturn)
/* this returns fully expanded line from the lpLine Buffer */
{
    Dos9_EsCpy(lpReturn, lpLine); /* Copy the content of the line in the buffer */
    Dos9_DelayedExpand(lpReturn, bDelayedExpansion); /* Expands the content of the specified  line */

    return NULL;
}
