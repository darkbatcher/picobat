#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>

#include "Dos9_Clone.h"
#include "Dos9_Core.h"
#include "../command/Dos9_Ask.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_Lang.h"

//#define DOS9_DBG_MODE
#include "Dos9_Debug.h"

#include "../../config.h"

#if defined(DOS9_USE_LIBCU8)
#include <libcu8.h>
#endif /* DOS9_USE_LIBCU8*/


void Dos9_PrintCompletionList(FILELIST* files);
int Dos9_GetLongestCommonMatch(FILELIST* files, int min);

#ifndef WIN32
#include "../linenoise/linenoise.h"

void Dos9_LinenoiseCompletionHandler(const char *buf, linenoiseCompletions *lc)
{
    FILELIST* files;
    ESTR* name = Dos9_EsInit();
    int len, match;

    Dos9_EsCpy(name, in);
    Dos9_EsToFullPath(name);
    len = strlen(name->str);
    Dos9_EsCat(name, "*");

    if ((files = Dos9_GetMatchFileList(name->str,
                                    DOS9_SEARCH_DEFAULT
                                    | DOS9_SEARCH_NO_PSEUDO_DIR)) == NULL) {

        *subst = NULL;
        goto end;

    }

    match = Dos9_GetLongestCommonMatch(files, len);

    if (subst == NULL) {

        Dos9_PrintCompletionList(files);
        Dos9_OutputPrompt();

    } else if (match == -1) {
        /* only one match */
        *subst = strdup(files->lpFileName + len);

    } else if (match > len) {
        /* a partial match ! */

        *subst = malloc(match - len + 1);
        if (*subst)
            snprintf(*subst, match - len + 1, "%s",  files->lpFileName + len);

    } else
        *subst = (char*)-1;

end:
    Dos9_EsFree(name);
    Dos9_FreeFileList(files);
}


#endif // WIN32





void Dos9_InitCompletion(void)
{
#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
    libcu8_completion_handler = Dos9_CompletionHandler;
    libcu8_completion_handler_free = Dos9_CompletionHandlerFree;
#else
    /* linenoiseSetCompletionCallBack(Dos9_CompletionHandler); */
#endif /* WIN32 */
}


#ifdef WIN32
int Dos9_CompletionGetCols(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    return csbi.dwSize.X - 1;
}
#endif /* WIN32 */


void Dos9_PrintCompletionList(FILELIST* files)
{
    char *name, *pch;
    size_t count, max = 0, cols=80;
    FILELIST *item = files;
    int ok;

#ifdef WIN32
    cols = Dos9_CompletionGetCols();
#endif /* WIN32 */

    fputs(DOS9_NL, fOutput);

    count = 0;
    while (item) {

        pch = item->lpFileName;
        name = item->lpFileName;

        while (*pch) {

            if (*pch == '/' || *pch == '\\')
                name = pch + 1;

            pch ++;

        }

        if (strlen(name) > max)
            max = strlen(name);

        count ++;

        item = item->lpflNext;
    }

    if (count > 20) {

        ok = Dos9_AskConfirmation(DOS9_ASK_YN | DOS9_ASK_DEFAULT_N
                                  | DOS9_ASK_INVALID_REASK,
                                  lpManyCompletionOptions, count);

        if (ok == DOS9_ASK_NO) {
            fputs(DOS9_NL, fOutput);
            return;
        }

    }

    if (max > cols) {
        /* some utterly lengthy filename is messing with us
           apparently */

        max = cols - 1;

    }

    count = 0;
    item = files;

    while (item) {

        pch = item->lpFileName;
        name = item->lpFileName;

        while (*pch) {

            if (*pch == '/' || *pch == '\\')
                name = pch + 1;

            pch ++;

        }

        fprintf(fOutput, "%-*s", max + 1, name);
        count ++;

        if (!(count % (cols/(max + 1))))
            fputs(DOS9_NL, fOutput);

        item = item->lpflNext;
    }

    if ((count % (cols/(max + 1))))
        fputs(DOS9_NL, fOutput);

    fputs(DOS9_NL, fOutput);
}

int Dos9_GetLongestCommonMatch(FILELIST* files, int min)
{
    char* p = files->lpFileName;
    int longest = -1, match;

    files = files->lpflNext;

    while (files) {

        match = 0;

        while (p[match] != 0
               && p[match] == files->lpFileName[match])
            match ++;

        if (longest == -1
            || longest > match)
            longest = match;


        if (longest == min)
            return min;

        files = files->lpflNext;

    }

    return longest;
}

__cdecl void Dos9_CompletionHandler(const char* in, const char** subst)
{
    FILELIST* files;
    ESTR* name = Dos9_EsInit();
    int len, match;

    Dos9_EsCpy(name, in);
    Dos9_EsToFullPath(name);
    len = strlen(name->str);
    Dos9_EsCat(name, "*");

    if ((files = Dos9_GetMatchFileList(name->str,
                                    DOS9_SEARCH_DEFAULT
                                    | DOS9_SEARCH_NO_PSEUDO_DIR)) == NULL) {

        *subst = NULL;
        goto end;

    }

    match = Dos9_GetLongestCommonMatch(files, len);

    if (subst == NULL) {

        Dos9_PrintCompletionList(files);
        Dos9_OutputPrompt();

    } else if (match == -1) {
        /* only one match */
        *subst = strdup(files->lpFileName + len);

    } else if (match > len) {
        /* a partial match ! */

        *subst = malloc(match - len + 1);
        if (*subst)
            snprintf(*subst, match - len + 1, "%s",  files->lpFileName + len);

    } else
        *subst = (char*)-1;

end:
    Dos9_EsFree(name);
    Dos9_FreeFileList(files);
}

__cdecl void Dos9_CompletionHandlerFree(char* p)
{
    free(p);
}
