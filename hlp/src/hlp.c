/*
 *
 *   HLP - A free cross platform manual manager - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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
#include <locale.h>

#ifdef WIN32

    #include "libintl.h"
    #include "iconv.h"

#else

    #include <libintl.h>
    #include <iconv.h>

#endif

#include "hlp_showconst.h"
#include "libDos9.h"
#include "hlp_load.h"
#include "hlp_enc.h"
#include "hlp_show.h"

int main (int argc, char* argv[])
{

    char lpPageName[FILENAME_MAX];
        /* a pointer to the page fileName */

    char lpPageTitle[FILENAME_MAX];
        /* a pointer to the page title */

    char lpShareBase[FILENAME_MAX];
        /* the basis of the share folder */
    char lpLocaleBase[FILENAME_MAX];
        /* the path for the lang for gettext*/

    char* lpDos9Base;
    char lpLang[25]="en";

    char lpExeBase[FILENAME_MAX];

    char lpCmd[80];
        /* a pointer to the command */

    char lpEnc[15]="ASCII";

    HLPPAGE* lpPage;
        /* a pointer to the man page */

    HLPLINKS* lpLinks;
        /* a pointer to the list of links of man pages */

    int i=0;
    int iLoadFromFile=FALSE;
    char* lpName=NULL;

	if (Dos9_LibInit() == -1) {

        puts("Error : Unable to load LibDos9. Exiting ...");
        exit(-1);
    }

    /* initializing gettext lib */
    Dos9_SetNewLineMode(DOS9_NEWLINE_LINUX);
    Dos9_GetConsoleEncoding(lpEnc, sizeof(lpEnc));

    /* if the var %DOS9_PATH% is defined in the currente environment */

    lpDos9Base=getenv("DOS9_PATH");
    if (lpDos9Base) {

        /* if we are actually running dos9 */
        snprintf(lpShareBase, sizeof(lpShareBase), "%s/share", lpDos9Base);

    } else {

        /* if we are not running Dos9, then we get the file name
           of the executable. */

        Dos9_GetExePath(lpExeBase, sizeof(lpExeBase));

        snprintf(lpShareBase, sizeof(lpShareBase), "%s/share", lpExeBase);

    }

    /* if the %LANGUAGE% is declared we set lang */

    if (getenv("LANGUAGE")) {

        snprintf(lpLang, sizeof(lpLang), "%s", getenv("LANGUAGE"));

    }

    snprintf(lpLocaleBase, sizeof(lpLocaleBase), "%s/locale", lpShareBase);

    if (argc<1) {

        fprintf(stderr, gettext("HLP :: Bad command line"));

        return -1;
    }

    for (i=0;argv[i];i++) {

        if (!stricmp(argv[i], "/f")) {

            iLoadFromFile=TRUE;

        } else if (!strcmp(argv[i], "/l")) {

            i++;

            if  (!argv[i]) {

                fprintf(stderr, gettext("HLP :: Bad command line"));
                return -1;

            }

            strncpy(lpLang, argv[i], sizeof(lpLang));
            lpLang[sizeof(lpLang)-1]='\0';

        } else {

            lpName=argv[i];

        }

    }

    if ((lpDos9Base=strchr(lpLang, '_'))) {

        /* select only the first part
           of the language */

        *lpDos9Base='\0';

    }

    if (iLoadFromFile) {

        snprintf(lpPageName, sizeof(lpPageName),  "%s", lpName);

    } else {

        snprintf(lpPageName, sizeof(lpPageName), "%s/doc/%s/man/%s", lpShareBase, lpLang, lpName);

    }

    bindtextdomain("hlp", lpLocaleBase);
    bind_textdomain_codeset("hlp", lpEnc);
    textdomain("hlp");


    /* if the file does not exist, fallback
       to the english version */



    if (!Dos9_FileExists(lpPageName) && stricmp(lpLang, "en")) {

        snprintf(lpPageName, sizeof(lpPageName), "%s/doc/en/man/%s", lpShareBase, lpName);

        fprintf(stderr, gettext("HLP :: Error : Unknown page ``%s'' for ``%s'' language. Fallback to english.\n"), lpName, lpLang);

        if (!Dos9_FileExists(lpPageName)) {

            fprintf(stderr, gettext("HLP :: Error : Can't find ``%s'' man page.\n"), lpName);
            exit(-1);

        }

    }

    Hlp_LoadManPage(lpPageName, lpPageTitle, FILENAME_MAX, &lpPage, &lpLinks);

        /* clears the console screen */
    Dos9_ClearConsoleScreen();

    Dos9_SetConsoleTitle(lpPageTitle);

    do {

        Dos9_SetConsoleCursorPosition((CONSOLECOORD){0,0});
        Hlp_ShowHeading(lpPageTitle);
        Dos9_SetConsoleCursorPosition((CONSOLECOORD){0,2});
        Hlp_ShowPage(lpPage, HLP_TEXT_LENGTH); // Shows the text
        Hlp_GetCommand(lpCmd, 80);

    } while (Hlp_RunCommand(lpCmd, lpPageName, FILENAME_MAX , lpPageTitle, FILENAME_MAX, &lpPage, &lpLinks));

    Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
    Dos9_ClearConsoleScreen();

    Hlp_FreeManPage(lpPage);

    Hlp_FreeManPageLinks(lpLinks);
    printf("\n");
    _exit(0);

    return 0;
}
