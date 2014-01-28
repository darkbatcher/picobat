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
#include <time.h>

#include <libintl.h>
#include <iconv.h>

#include "hlp_showconst.h"

#include "libDos9.h"
#include "hlp_load.h"
#include "hlp_show.h"

void Hlp_ShowPage(HLPPAGE* lpPage, int iLineNb)
{

    while (lpPage && iLineNb) {

        Hlp_ShowLine(lpPage->lpLineContent);

        lpPage=lpPage->lpNextLine;
        iLineNb--;
    }

}

void Hlp_ShowLine(HLPELEMENT* lpLine)
{

    Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);

    while (lpLine) {

        switch (lpLine->iType) {

            case HLP_ELEMENT_HEADING:

                Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_IMAGENTA);
                break;

            case HLP_ELEMENT_CODE:

                Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_IGREEN);
                break;

            case HLP_ELEMENT_LINK:

                Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_IBLUE);
                break;

            case HLP_ELEMENT_EMPHASIS:

                Dos9_SetConsoleTextColor(DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_ICYAN);

            case HLP_ELEMENT_TEXT:;
        }

        printf("%s", lpLine->lpContent);

        lpLine=lpLine->lpNext;

        Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);

    }
}

void Hlp_ShowHeading(char* lpTitle)
{
    char lpTotalBuf[80];
    char lpMargin[80]="";
    char lpName[80]="";
    int i;

    size_t iTitleLength=strlen(lpTitle);

    int iMarginWidth=(79-iTitleLength)/2-sizeof("[HLP 0.1.0.0] ");

    if (iMarginWidth>0) {

        for (i=0;i<=iMarginWidth;i++) {
            lpMargin[i]=' ';
        }

        lpMargin[i]='\0';

        snprintf(lpName, sizeof(lpName), "%s", lpTitle);

    } else {

        snprintf(lpName, sizeof(lpName)-2*sizeof("[HLP 1.0.0.1] ")-2, "%s", lpTitle);

    }

    snprintf(lpTotalBuf, sizeof(lpTotalBuf), "[HLP 1.0.0.1] %s%s%s              ", lpMargin, lpName, lpMargin);


    Dos9_SetConsoleTextColor(DOS9_BACKGROUND_IBLACK | DOS9_FOREGROUND_IWHITE);
    Dos9_SetConsoleCursorPosition((CONSOLECOORD){0,0});

    printf("%s", lpTotalBuf);


    Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
    Dos9_SetConsoleCursorPosition((CONSOLECOORD){0,1});

}

void Hlp_GetCommand(char* lpCmd, size_t iSize)
{
    static char lpLast[80]="";
    char* lpToken;

    if (lpCmd==NULL) {

        lpLast[0]='\0';
        return;

    }

    Dos9_SetConsoleTextColor(DOS9_FOREGROUND_IWHITE | DOS9_BACKGROUND_IBLACK);
    Dos9_SetConsoleCursorPosition((CONSOLECOORD){0,HLP_BAR_Y});


    printf("    :                                                                          \n");
    printf(gettext("U(P) : scroll up  D(OWN) : scroll down  L(NK) : Follow link  E(XIT) : exit     "));


    Dos9_SetConsoleTextColor(DOS9_BACKGROUND_WHITE | DOS9_FOREGROUND_BLACK);
    Dos9_SetConsoleCursorPosition((CONSOLECOORD){5,HLP_BAR_Y});

    printf("                                                                         ");

    Dos9_SetConsoleCursorPosition((CONSOLECOORD){6,HLP_BAR_Y});

    fgets(lpCmd, iSize, stdin);

    if (*lpCmd!='\n') {

        strncpy(lpLast, lpCmd, 80);
        lpLast[79]='\0';

    } else {

        strncpy(lpCmd, lpLast, iSize);
        lpCmd[iSize-1]='\0';

    }
}

int Hlp_RunCommand(char* lpCmd, char* lpPageName, size_t iNameSize, char* lpTitle, size_t iSize, HLPPAGE** lpPage, HLPLINKS** lpLinks)
{
    clock_t tBegin;
    char lpFile[FILENAME_MAX];
    char *lpToken;
    int iLinkPosition;
    int iLine, iTotalNb;

    HLPLINKS* lpLinksTmp, *lpNewLinks;

    if (!stricmp(lpCmd, "down\n") || !stricmp(lpCmd, "d\n")) {

        if ((*lpPage)->lpNextLine) {

            *lpPage=(*lpPage)->lpNextLine;

        }


    } else if (!stricmp(lpCmd, "up\n") || !stricmp(lpCmd, "u\n")) {

        if ((*lpPage)->lpPreviousLine) {

            *lpPage=(*lpPage)->lpPreviousLine;

        }

    } else if (!strnicmp(lpCmd, "lnk ", 4) || !strnicmp(lpCmd, "l ", 2)) {

        lpCmd+=4;

        iLinkPosition=atoi(lpCmd);
        lpLinksTmp=*lpLinks;

        while (lpLinksTmp) {


            if (lpLinksTmp->iLinkNb == iLinkPosition) {

                Hlp_FreePage(*lpPage);

                Hlp_MakePath(lpFile, lpPageName, lpLinksTmp->lpTarget, FILENAME_MAX);

                Hlp_LoadManPage(lpFile, lpTitle, iSize, lpPage, &lpNewLinks);

                Hlp_FreeManPageLinks(*lpLinks);

                snprintf(lpPageName, iNameSize, "%s", lpFile);

                Hlp_GetCommand(NULL, 0); /* purges the old command */

                *lpLinks=lpNewLinks;

                break;

            }

            lpLinksTmp=lpLinksTmp->lpNextLink;

        }

    } else if (!strnicmp(lpCmd, "open ", 5) || !strnicmp(lpCmd, "o ", 2)) {

        lpCmd+=5;

        if ((lpToken=strchr(lpCmd,'\n'))) {
            *lpToken='\0';
        }

        Hlp_FreePage(*lpPage);

        Hlp_MakePath(lpFile, lpPageName, lpCmd, FILENAME_MAX);

        Hlp_LoadManPage(lpFile, lpTitle, iSize, lpPage, &lpNewLinks);

        Hlp_FreeManPageLinks(*lpLinks);

        snprintf(lpPageName, iNameSize, "%s", lpFile);

        Hlp_GetCommand(NULL, 0); /* purges the old command */

        *lpLinks=lpNewLinks;


    } else if (!stricmp(lpCmd, "exit\n") || !stricmp(lpCmd, "e\n")
               || !stricmp(lpCmd, "q\n")
               ) {

        return 0;

    } else if (!stricmp(lpCmd, "pgdown\n")) {

        for (iLine=20;iLine && (*lpPage)->lpNextLine;iLine--) {

            (*lpPage)=(*lpPage)->lpNextLine;

        }

        /* fixme :

            sometimes the text under the page
            is not cleared

        */

    } else if (!stricmp(lpCmd, "pgup\n")) {

        for (iLine=20;iLine && (*lpPage)->lpPreviousLine;iLine--) {

            (*lpPage)=(*lpPage)->lpPreviousLine;

        }

    } else if (!stricmp(lpCmd, "info\n")) {

        Dos9_SetConsoleCursorPosition((CONSOLECOORD){5,HLP_BAR_Y});

        iLine=Hlp_GetLine(*lpPage);
        iTotalNb=Hlp_GetLineNb(*lpPage);

        printf("[fichier=\"%s\" ligne=%d %d%%]", lpPageName, iLine, iLine*100/iTotalNb);

        tBegin=clock();

        while (clock()-tBegin <= CLOCKS_PER_SEC);

    } else {


        Dos9_SetConsoleTextColor(DOS9_BACKGROUND_WHITE | DOS9_FOREGROUND_IRED);
        Dos9_SetConsoleCursorPosition((CONSOLECOORD){6,HLP_BAR_Y});

        printf(gettext("Error : bad command line (type ``HELP'' for avaliable commands)"));

        tBegin=clock();
        while (clock()-tBegin <= CLOCKS_PER_SEC);

    }

    return 1;
}

void Hlp_MakePath(char* lpNewPath, char* lpOldPath, char* lpRelative, size_t iSize)
{

    char *lpLastPathDelim=NULL, *lpToken;

    /* the file is designed to be relative path, thus we make a relative path before */

    strncpy(lpNewPath, lpOldPath, iSize);
    lpNewPath[iSize-1]='\0';

    /* we suppress the trailing filename */
    lpToken=lpNewPath;

    while (*lpToken) {

        if (*lpToken=='\\' || *lpToken=='/') {

            lpLastPathDelim=lpToken;

        }

        lpToken++;

    }

    if (lpLastPathDelim) {

        *lpLastPathDelim='\0';

    }

    /* now we are about to add new file name */

    if (!strncmp(lpRelative, "..\\", 3) || !strncmp(lpRelative, "../", 3)) {

        lpToken=lpNewPath;

        while (*lpToken) {

            if (*lpToken=='\\' || *lpToken=='/') {

                lpLastPathDelim=lpToken;

            }

            lpToken++;

        }

        if (lpLastPathDelim) {

            *lpLastPathDelim='\0';

        }

        lpRelative+=3;

    }

    if (!strncmp(lpRelative, "./", 2) || !strncmp(lpRelative, ".\\", 2)) {

        lpRelative+=2;

    }

    strncat(lpNewPath, "/", iSize-strlen(lpNewPath));
    lpNewPath[iSize-1]='\0';
    strncat(lpNewPath, lpRelative, iSize-strlen(lpNewPath));
    lpNewPath[iSize-1]='\0';

}


int Hlp_GetLine(HLPPAGE* lpPage)
{
    int iNb=0;

    while (lpPage) {

        iNb++;

        lpPage=lpPage->lpPreviousLine;

    }

    return iNb;
}

int Hlp_GetLineNb(HLPPAGE* lpPage)
{
    int iNb=Hlp_GetLine(lpPage);

    lpPage=lpPage->lpNextLine;

    while (lpPage) {

        iNb++;

        lpPage=lpPage->lpNextLine;

    }

    return iNb;
}
