#include <stdio.h>

#include "../libDos9.h"
#include "../../config.h"

#ifdef LIBDOS9_NO_CONSOLE

void Dos9_ClearConsoleScreen(FILE *f)
{
}

void Dos9_SetConsoleColor(FILE *f, COLOR cColor)
{
}

void Dos9_SetConsoleTextColor(FILE* f, COLOR cColor) {
}

LIBDOS9 void Dos9_SetConsoleCursorPosition(FILE* f, CONSOLECOORD iCoord)
{
}

LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(FILE* f)
{
    return (CONSOLECOORD){0,0};
}

LIBDOS9 void Dos9_SetConsoleCursorState(FILE* f, int bVisible, int iSize)
{
}

void Dos9_SetConsoleTitle(FILE* f, char* lpTitle)
{
}
void Dos9_ClearConsoleLine(FILE* f)
{
}

void Dos9_GetMousePos(FILE* f, char on_move, CONSOLECOORD* coords, int* type)
{
    type = 0;
    coords->X = 0;
    coords->Y = 0;
}

#endif