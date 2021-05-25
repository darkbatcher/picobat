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

#ifndef PBAT_MODULE_READ_H
#define PBAT_MODULE_READ_H

#include <stdio.h>

#include "pBat_ReadScript.h"

typedef struct INPUT_FILE {
	char  lpFileName[FILENAME_MAX];
	int   iPos;
	int   bEof;
	struct batch_script_t batch;
} INPUT_FILE;

int pBat_GetLine(ESTR* lpesLine, INPUT_FILE* pIn);
int pBat_GetLine_Cmdly(ESTR* lpesLine, INPUT_FILE* pIn);
int pBat_CheckBlocks(ESTR* lpesLine);
void pBat_RmTrailingNl(char* lpLine);

int pBat_UpdateScript(INPUT_FILE* pIn);
#endif
