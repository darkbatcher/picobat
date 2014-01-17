#include "libDos9.h"
#include "Dos9_Core.h"
#include "Dos9_ShowIntro.h"
#include "../lang/Dos9_Lang.h"

#ifdef WIN32
#include <windows.h>

void Dos9_InitConsole(void)
{
    CONSOLE_CURSOR_INFO cciCursorInfo;
    cciCursorInfo.dwSize=100;
    cciCursorInfo.bVisible=TRUE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cciCursorInfo);
}

#else

void Dos9_InitConsole(void) {}

#endif


void Dos9_PrintIntroduction(void)
{
    char lpDesc[]="                                         ";
    char lpAuth[]="                                      ";

    size_t iSize=sizeof(lpDesc);

     #ifdef DOS9_BUILDCOMMENTS

        #define DOS9_DESC_TOKEN "Dos9 [" DOS9_VERSION "] - " DOS9_BUILDCOMMENTS

     #else

        #define DOS9_DESC_TOKEN "Dos9 [" DOS9_VERSION "]"

     #endif

    if (sizeof(DOS9_DESC_TOKEN)-1 < iSize)
        iSize=sizeof(DOS9_DESC_TOKEN)-1;

    strncpy(lpDesc, DOS9_DESC_TOKEN , iSize);


    iSize=sizeof(lpAuth);
    if (sizeof(DOS9_AUTHORS)-1 < iSize)
        iSize=sizeof(DOS9_AUTHORS)-1;

    strncpy(lpAuth, DOS9_AUTHORS, iSize);

    Dos9_SetConsoleTextColor(DOS9_BACKGROUND_IBLUE | DOS9_FOREGROUND_IYELLOW);

    printf("  ______   _______  _______   _____                                            \n\
 (  __  \\ (  ___  )(  ____ \\ / ___ \\  %s\n\
 | (  \\  )| (   ) || (    \\/( (   ) ) Copyright (c) 2010-" DOS9_BUILDYEAR "                  \n\
 | |   ) || |   | || (_____ ( (___) |    %s\n\
 | |   | || |   | |(_____  ) \\____  |                                          \n\
 | |   ) || |   | |      ) |      ) | This is free software, you can modify    \n\
 | (__/  )| (___) |/\\____) |/\\____) ) and/or redistribute it under the terms   \n\
 (______/ (_______)\\_______)\\______/  of the GNU Genaral Public License V3     \n\
                                                                               \n",
           lpDesc,
           lpAuth
           );
    Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
    printf("\n");
}
