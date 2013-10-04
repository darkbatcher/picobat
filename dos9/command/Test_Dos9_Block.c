#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Dos9_CmdLib.h"

int main (void) {
    char lpLine[]="this (example of blocks\n\
    echo romain)\n\
    test";
    char* lpStart, *lpEnd;
    if (lpStart=Dos9_GetNextBlock(lpLine+4, &lpEnd)) {
        *lpEnd='\0';
        puts (lpStart);
        puts("==");
        puts(lpEnd+1);
    }
    return 0;
}
