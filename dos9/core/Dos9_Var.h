#ifndef DOS9_VAR_H
#define DOS9_VAR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include "LibDos9.h"

#define LOCAL_VAR_BLOCK_SIZE 128

#ifndef WIN32
    #define _MAX_DRIVE FILENAME_MAX
    #define _MAX_DIR FILENAME_MAX
    #define _MAX_FNAME FILENAME_MAX
    #define _MAX_EXT FILENAME_MAX

    int Dos9_PutEnv(char* lpEnv);
    void Dos9_SplitPath(char* lpPath, char* lpDisk, char* lpDir, char* lpName, char* lpExt);

#else
    #define Dos9_SplitPath _splitpath
    #define Dos9_PutEnv(a) putenv(a)
#endif

#define DOS9_ALL_PATH    0
#define DOS9_DISK_LETTER 1
#define DOS9_PATH_PART   2
#define DOS9_FILENAME    3
#define DOS9_EXTENSION   4
#define DOS9_FILESIZE    5
#define DOS9_TIME        6
#define DOS9_ATTR        7

#define DOS9_VAR_MAX_OPTION 16


typedef char* LOCAL_VAR_BLOCK;

int Dos9_InitVar(char* lpArray[]);
int Dos9_SetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char cVarName, char* cVarContent);
LOCAL_VAR_BLOCK* Dos9_GetLocalBlock(void);
void Dos9_FreeLocalBlock(LOCAL_VAR_BLOCK* lpBlock);
char* Dos9_GetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char* lpName, ESTR* lpRecieve);
int  Dos9_GetVar(char* lpName, ESTR* lpRecieve);
char* Dos9_GetLocalVarPointer(LOCAL_VAR_BLOCK* lpvBlock, char cVarName);

#define DOS9_TEST_VARNAME(cVar) \
        if (((cVar) & 0x80) || ((cVar) <= 0x20)) { \
            /* if cVarName appears to be non-strict ascii format, execpted white
               spaces, and nul character */ \
            if (!(isspace(cVar)) && (cVar)) \
                Dos9_ShowErrorMessage(DOS9_SPECIAL_VAR_NON_ASCII, cVar, FALSE); \
            return NULL; \
        }

#endif
