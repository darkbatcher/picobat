#ifndef DOS9_CMD_LIBRARY
#define DOS9_CMD_LIBRARY

#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdlib.h>

#include "LibDos9.h"
#ifndef DOS9API_INCLUDED
    #define TRUE 1
    #define FALSE 0
#endif

#define DOS9_CMDLIB

DOS9_CMDLIB char* Dos9_GetNextParameterEs(char* lpLine, ESTR* lpReturn);
DOS9_CMDLIB char* Dos9_GetNextParameter(char* lpLine, char* lpResponseBuffer, int iLength);
DOS9_CMDLIB int   Dos9_GetParamArrayEs(char* lpLine, ESTR** lpArray, size_t iLenght);
DOS9_CMDLIB char* Dos9_GetNextBlock(char* lpLine, char** lpNextPart);
DOS9_CMDLIB char* Dos9_GetEndOfLine(char* lpLine, ESTR* lpReturn);

//#include "cmdlib/Dos9_CmdLibFiles.c"

#endif
