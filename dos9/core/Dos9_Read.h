#ifndef DOS9_MODULE_READ_H
#define DOS9_MODULE_READ_H

#include <stdio.h>

typedef struct INPUT_FILE {
	char  lpFileName[FILENAME_MAX];
	int   iPos;
	int   bEof;
} INPUT_FILE;

int Dos9_GetLine(ESTR* lpesLine, INPUT_FILE* pIn);
int Dos9_CheckBlocks(ESTR* lpesLine);
void Dos9_RmTrailingNl(char* lpLine);

#endif
