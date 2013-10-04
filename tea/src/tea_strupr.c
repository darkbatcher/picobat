#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "LibDos9.h"

char* tea_Strupr(char* lpChar)
{
    char* const lpCharBegin=lpChar;
  
    while (*lpChar) {

        if (isalpha(*lpChar)) *lpChar=toupper(*lpChar);

        lpChar=Dos9_GetNextChar(lpChar);

    }

    return lpCharBegin;
}
