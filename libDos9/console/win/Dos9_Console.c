#ifdef WIN32

#include "../../LibDos9.h"

void Dos9_ClearConsoleScreen(void)
{
    COORD coordScreen = { 0, 0 };    /* here's where we'll home the
                                        cursor */
    HANDLE hConsole=GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
    DWORD dwConSize;                 /* number of character cells in
                                        the current buffer */

    /* get the number of character cells in the current buffer */
    GetConsoleScreenBufferInfo( hConsole, &csbi );
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    /* fill the entire screen with blanks */

    FillConsoleOutputCharacter( hConsole, (TCHAR) ' ',
       dwConSize, coordScreen, &cCharsWritten );

    /* get the current text attribute */

    GetConsoleScreenBufferInfo( hConsole, &csbi );

    /* now set the buffer's attributes accordingly */

    FillConsoleOutputAttribute( hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten );

    /* put the cursor at (0, 0) */
    SetConsoleCursorPosition( hConsole, coordScreen );
    return;
 }


void Dos9_SetConsoleColor(COLOR cColor)
{
    CONSOLE_SCREEN_BUFFER_INFO csbiScreenInfo;
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    int iSize;
    const COORD cBegin={0,0};
    GetConsoleScreenBufferInfo(hOutput, &csbiScreenInfo);
    iSize=csbiScreenInfo.dwSize.X * csbiScreenInfo.dwSize.Y;
    FillConsoleOutputAttribute(hOutput, cColor, iSize, cBegin, (PDWORD)&iSize);
    SetConsoleTextAttribute(hOutput, cColor);
}

void Dos9_SetConsoleTextColor(COLOR cColor) {
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hOutput, cColor);
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord)
{
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOutput, (COORD)iCoord);
}

LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(void)
{
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOutput, &csbi);
    return csbi.dwCursorPosition;
}

LIBDOS9 void Dos9_SetConsoleCursorState(int bVisible, int iSize)
{
    HANDLE hOutput=GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cciInfo;
    cciInfo.bVisible=(BOOL)bVisible;
    cciInfo.dwSize=iSize;
    SetConsoleCursorInfo(hOutput, &cciInfo);
}

void Dos9_SetConsoleTitle(char* lpTitle)
{
    SetConsoleTitle(lpTitle);
}

#endif
