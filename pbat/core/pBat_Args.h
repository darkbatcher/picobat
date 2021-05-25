/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#ifndef PBAT_ARGS_H
#define PBAT_ARGS_H

#include "pBat_Core.h"

/* a structure that contains boundaries of a block */

typedef struct BLOCKINFO {
	char* lpBegin;
	char* lpEnd;
} BLOCKINFO;

typedef struct PARAMLIST {
    ESTR* param;
    struct PARAMLIST* next;
} PARAMLIST;

int   pBat_GetParameterPointers(char** lpPBegin, char** lpPEnd,
				 const char* lpDelims, const char* lpLine);

#define PBAT_DELIMITERS " ;,\t\"\n"

#define pBat_GetNextParameterEs(lpLine, lpReturn) \
    pBat_GetNextParameterEsD(lpLine, lpReturn, PBAT_DELIMITERS)

char* pBat_GetNextParameterEsD(char* lpLine, ESTR* lpReturn, const
					char* lpDelims);

char* pBat_GetNextParameter(char* lpLine, char* lpResponseBuffer, int iLength);

struct PARAMLIST* pBat_GetParamList(char* lpLine);
void pBat_FreeParamList(struct PARAMLIST* list);

char* pBat_GetNextBlockEs(char* lpLine, ESTR* lpReturn);

char* pBat_GetNextBlock(char* lpLine, BLOCKINFO* lpbkInfo);

char* pBat_GetBlockLine(char* pLine, BLOCKINFO* pBk);

char* pBat_GetEndOfLine(char* lpLine, ESTR* lpReturn);

#endif // PBAT_ARGS_H
