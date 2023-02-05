/*
 *
 *   libpBat - The pBat project
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

#ifndef LIBPBAT_INT_INCLUDED
#define LIBPBAT_INT_INCLUDED


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

#include "libpBat.h"


#ifdef WIN32
    #ifndef S_IRUSR
		#define S_IRUSR _S_IREAD
	#endif
	#ifndef S_IWUSR
		#define S_IWUSR _S_IWRITE
	#endif
#endif


void _pBat_Estr_Close(void);
int _pBat_Estr_Init(void);
int _pBat_Thread_Init(void);
void _pBat_Thread_Close(void);

typedef struct {
	char bStat;
	int iInput;
	void(*pCallBack)(FILELIST*);
} FILEPARAMETER,*LPFILEPARAMETER;

#ifdef WIN32
int                 pBat_GetFileAttributes(const char* file);
#endif

int                     _pBat_FillCommandList(LPCOMMANDLIST lpclList, LPCOMMANDINFO lpciCommandInfo);
int                     _pBat_PutSeed(LPCOMMANDINFO lpciCommandInfo, int iSegBottom, int iSegTop, LPCOMMANDLIST* lpclList);
int                     _pBat_Sort(const void* ptrS, const void* ptrD);
size_t                  _pBat_CountListItems(LPCOMMANDLIST lpclList);
LPCOMMANDINFO           _pBat_FillInfoFromList(LPCOMMANDINFO lpCommandInfo, LPCOMMANDLIST lpCommandList);

char*               _pBat_SeekPatterns(char* lpSearch, char* lpPattern);
char*               _pBat_SeekCasePatterns(char* lpSearch, char* lpPattern);
int                 _pBat_SplitMatchPath(const char* lpPathMatch, char* lpStaticPart, size_t iStaticSize,  char* lpMatchPart, size_t iMatchSize);
int                 _pBat_GetMatchPart(const char* lpRegExp, char* lpBuffer, size_t iLength, int iLvl);
int                 _pBat_GetMatchDepth(char* lpRegExp);
int                 _pBat_MakePath(char* lpPathBase, char* lpPathEnd, char* lpBuffer, int iLength);
char*               _pBat_GetFileName(char* lpPath);
int                 _pBat_FreeFileList(LPFILELIST lpflFileList);
LPFILELIST          _pBat_WaitForFileList(LPFILEPARAMETER lpParam);
int                 _pBat_WaitForFileListCallBack(LPFILEPARAMETER lpParam);
LPFILELIST          _pBat_SeekFiles(char* lpDir, char* lpRegExp, int iLvl, int iMaxLvl, int iOutDescriptor, int iSearchFlag);

int _pBat_IsFollowingByte(const char* lpChar);

#endif
