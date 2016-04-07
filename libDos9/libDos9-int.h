/*
 *
 *   libDos9 - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef LIBDOS9_INT_INCLUDED
#define LIBDOS9_INT_INCLUDED


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <dirent.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef WIN32
    #define S_IREAD _S_IREAD
    #define O_IWRITE _S_IWRITE
    #define Dos9_GetFileAttributes(lpcstr) GetFileAttributes(lpcstr)
#else
    #define Dos9_GetFileAttributes(lpcstr)
#endif


void _Dos9_Estr_Close(void);
int _Dos9_Estr_Init(void);
int _Dos9_Thread_Init(void);
void _Dos9_Thread_Close(void);

typedef struct {
	char bStat;
	int iInput;
	void(*pCallBack)(FILELIST*);
} FILEPARAMETER,*LPFILEPARAMETER;


int                     _Dos9_FillCommandList(LPCOMMANDLIST lpclList, LPCOMMANDINFO lpciCommandInfo);
int                     _Dos9_PutSeed(LPCOMMANDINFO lpciCommandInfo, int iSegBottom, int iSegTop, LPCOMMANDLIST* lpclList);
int                     _Dos9_Sort(const void* ptrS, const void* ptrD);
size_t                  _Dos9_CountListItems(LPCOMMANDLIST lpclList);
LPCOMMANDINFO           _Dos9_FillInfoFromList(LPCOMMANDINFO lpCommandInfo, LPCOMMANDLIST lpCommandList);

char*               _Dos9_SeekPatterns(char* lpSearch, char* lpPattern);
char*               _Dos9_SeekCasePatterns(char* lpSearch, char* lpPattern);
int                 _Dos9_SplitMatchPath(const char* lpPathMatch, char* lpStaticPart, size_t iStaticSize,  char* lpMatchPart, size_t iMatchSize);
int                 _Dos9_GetMatchPart(const char* lpRegExp, char* lpBuffer, size_t iLength, int iLvl);
int                 _Dos9_GetMatchDepth(char* lpRegExp);
int                 _Dos9_MakePath(char* lpPathBase, char* lpPathEnd, char* lpBuffer, int iLength);
char*               _Dos9_GetFileName(char* lpPath);
int                 _Dos9_FreeFileList(LPFILELIST lpflFileList);
LPFILELIST          _Dos9_WaitForFileList(LPFILEPARAMETER lpParam);
int                 _Dos9_WaitForFileListCallBack(LPFILEPARAMETER lpParam);
LPFILELIST          _Dos9_SeekFiles(char* lpDir, char* lpRegExp, int iLvl, int iMaxLvl, int iOutDescriptor, int iSearchFlag);

int _Dos9_IsFollowingByte(const char* lpChar);

#endif
