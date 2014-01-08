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
    Dos9_SetConsoleTextColor(DOS9_BACKGROUND_IBLUE | DOS9_FOREGROUND_IYELLOW);
    puts("  ______   _______  _______   _____                                            \n\
 (  __  \\ (  ___  )(  ____ \\ / ___ \\  Dos9 [" DOS9_VERSION "] " DOS9_BUILDCOMMENTS "\n\
 | (  \\  )| (   ) || (    \\/( (   ) ) Copyright (c) 2010-" DOS9_BUILDYEAR "                  \n\
 | |   ) || |   | || (_____ ( (___) |   " DOS9_AUTHORS D0S9_AUTHORSPAD "\n\
 | |   | || |   | |(_____  ) \\____  |                                          \n\
 | |   ) || |   | |      ) |      ) | This is free software, you can modify    \n\
 | (__/  )| (___) |/\\____) |/\\____) ) and/or redistribute it under the terms   \n\
 (______/ (_______)\\_______)\\______/  of the GNU Genaral Public License V3     \n\
                                                                               ");
    Dos9_SetConsoleTextColor(DOS9_COLOR_DEFAULT);
    printf("\n");
}
