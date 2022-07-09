#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#include <unistd.h>
#include <sys/types.h>

#include "pBat_Clone.h"
#include "pBat_Core.h"
#include "../command/pBat_Ask.h"

#include "../errors/pBat_Errors.h"
#include "../lang/pBat_Lang.h"

//#define PBAT_DBG_MODE
#include "pBat_Debug.h"

#include "../../config.h"

#if defined(PBAT_USE_LIBCU8)
#include <libcu8.h>
#endif /* PBAT_USE_LIBCU8*/

#ifndef WIN32
#include "../linenoise/linenoise.h"
#endif /* WIN32 */

void pBat_InitCompletion(void)
{
#if defined(WIN32) && defined(PBAT_USE_LIBCU8)
    libcu8_completion_handler = pBat_CompletionHandler;
    libcu8_completion_handler_free = pBat_CompletionHandlerFree;
#elif !defined(WIN32) && !defined(PBAT_NO_LINENOISE)
    linenoiseSetCompletionCallback(pBat_CompletionHandler);
#endif /* WIN32 */
}


#ifdef WIN32
int pBat_CompletionGetCols(void)
{
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);

    return csbi.dwSize.X - 1;
}

#define COMPLETION_READ NULL
#elif defined(PBAT_NO_LINENOISE)
#define COMPLETION_READ NULL
#else

/* A read function that takes into account the fact that
   when using linenoise, the terminal is in raw mode,
   requiring read characters to be printed explicitly in
   the terminal */
void pBat_CompleteReadRaw(char* buffer, size_t size)
{

    int i = 0;
    char c;

    while (1) {

        read(fileno(fInput), &c, 1);

        if (c == '\r'
            || c == '\n')
            break;

        if (i < size)
            buffer[i] = c;

        fputc(c, fError);

        i ++;
    }

    fputs("\r\n", fError);

    if (i < size)
        buffer[i] = '\0';
    else
        buffer[size - 1] = '\0';
}

#define COMPLETION_READ pBat_CompleteReadRaw
#endif /* WIN32 */

void pBat_PrintCompletionList(FILELIST* files)
{
    char *name, *pch;
    size_t count, max = 0, cols=80;
    FILELIST *item = files;
    int ok;

#ifdef WIN32
    cols = pBat_CompletionGetCols();
#endif /* WIN32 */

    fputs("\r\n", fOutput);

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

        do {

            ok = pBat_AskConfirmation(PBAT_ASK_YN | PBAT_ASK_DEFAULT_N,
                                   COMPLETION_READ, lpManyCompletionOptions,
                                   count);
#if !defined(WIN32)
            fputs("\r", fOutput);
#endif

        } while (ok == PBAT_ASK_INVALID);

        if (ok == PBAT_ASK_NO) {
            fputs(PBAT_NL, fOutput);
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
            fputs("\r\n", fOutput);

        item = item->lpflNext;
    }

    if ((count % (cols/(max + 1))))
        fputs("\r\n", fOutput);

    fputs("\r\n", fOutput);
}

int pBat_GetLongestCommonMatch(FILELIST* files, int min)
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

void pBat_CompletionHandler(const char* in, const char** subst)
{
    FILELIST* files;
    ESTR* name = pBat_EsInit_Cached();
    int len, match;

    pBat_EsCpy(name, in);
    pBat_EsToFullPath(name);
    len = strlen(name->str);
    pBat_EsCat(name, "*");

    if ((files = pBat_GetMatchFileList(name->str,
                                    PBAT_SEARCH_DEFAULT
                                    | PBAT_SEARCH_NO_PSEUDO_DIR)) == NULL) {

        *subst = NULL;
        goto end;

    }

    match = pBat_GetLongestCommonMatch(files, len);

    if (subst == NULL) {

        pBat_PrintCompletionList(files);

        if (bEchoOn) {
            if (lpAltPromptString)
                pBat_OutputPromptString(lpAltPromptString);
            else
                pBat_OutputPrompt();
        }

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
    pBat_EsFree_Cached(name);
    pBat_FreeFileList(files);
}

void pBat_CompletionHandlerFree(char* p)
{
    free(p);
}
