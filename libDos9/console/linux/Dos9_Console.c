#include "../../LibDos9.h"

#ifndef WIN32

void Dos9_ClearConsoleScreen(void)
{
    printf("\033[H\033[2J");
}

void Dos9_SetConsoleColor(COLOR cColor)
{
    Dos9_SetConsoleTextColor(cColor);
    printf("\033[H\033[2J");
}

void Dos9_SetConsoleTextColor(COLOR cColor)
{
    if (cColor & DOS9_FOREGROUND_INT) {
        printf("\033[1m");
        cColor^=DOS9_FOREGROUND_INT;
    } else {
        printf("\033[0m");
    }
    if (cColor & DOS9_BACKGROUND_DEFAULT) printf("\033[49m");
    if (cColor & DOS9_FOREGROUND_DEFAULT) printf("\033[39m");
    if ((cColor & DOS9_FOREGROUND_DEFAULT) && (cColor & DOS9_BACKGROUND_DEFAULT)) return;

    if (!(cColor & DOS9_BACKGROUND_DEFAULT)) {
        printf("\033[%dm",DOS9_GET_BACKGROUND_(cColor)+40);
    }
    if (!(cColor & DOS9_FOREGROUND_DEFAULT)) {
        printf("\033[%dm", DOS9_GET_FOREGROUND_(cColor)+30);
    }
}

void Dos9_SetConsoleTitle(char* lpTitle)
{
    printf("\033]0;%s\007", lpTitle);
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord)
{
    printf("\033[%d;%dH", iCoord.Y+1, iCoord.X+1);
}


LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(void)
{
    return (CONSOLECOORD){0,0};
}

LIBDOS9 void Dos9_SetConsoleCursorState(int bVisible, int iSize )
{
    if (bVisible) {
        printf("\033[25h");
    } else {
        printf("\033[25l");
    }
}

#endif
