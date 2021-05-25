#include <stdio.h>

#include "../libpBat.h"
#include "../../config.h"

#ifdef LIBPBAT_NO_CONSOLE

void pBat_ClearConsoleScreen(FILE *f)
{
}

void pBat_SetConsoleColor(FILE *f, COLOR cColor)
{
}

void pBat_SetConsoleTextColor(FILE* f, COLOR cColor) {
}

LIBPBAT void pBat_SetConsoleCursorPosition(FILE* f, CONSOLECOORD iCoord)
{
}

LIBPBAT CONSOLECOORD pBat_GetConsoleCursorPosition(FILE* f)
{
    return (CONSOLECOORD){0,0};
}

LIBPBAT void pBat_SetConsoleCursorState(FILE* f, int bVisible, int iSize)
{
}

void pBat_SetConsoleTitle(FILE* f, char* lpTitle)
{
}
void pBat_ClearConsoleLine(FILE* f)
{
}

void pBat_GetMousePos(FILE* f, char on_move, CONSOLECOORD* coords, int* type)
{
    type = 0;
    coords->X = 0;
    coords->Y = 0;
}

#endif