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
#ifndef PBAT_VAR_H
#define PBAT_VAR_H

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#include <libpBat.h>

#define LOCAL_VAR_BLOCK_SIZE 128

#define PBAT_ALL_PATH    0
#define PBAT_DISK_LETTER 1
#define PBAT_PATH_PART   2
#define PBAT_FILENAME    3
#define PBAT_EXTENSION   4
#define PBAT_FILESIZE    5
#define PBAT_TIME        6
#define PBAT_ATTR        7

#define PBAT_VAR_MAX_OPTION 16

#define pBat_GetLocalBlock() (LOCAL_VAR_BLOCK*)calloc(LOCAL_VAR_BLOCK_SIZE, sizeof(LOCAL_VAR_BLOCK))
#define pBat_IsLocalVarValid(c)  (c < 0x80 && c > 0x20)


typedef char* LOCAL_VAR_BLOCK;

int              pBat_InitVar(char* lpArray[]);

void             pBat_SetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char cVarName, char* cVarContent);

char*            pBat_GetLocalVar(LOCAL_VAR_BLOCK* lpvBlock, char* lpName, ESTR* lpRecieve);
int              pBat_GetVar(char* lpName, ESTR* lpRecieve);
char*            pBat_GetLocalVarPointer(LOCAL_VAR_BLOCK* lpvBlock, char cVarName);

LOCAL_VAR_BLOCK* pBat_DuplicateLocalVar(LOCAL_VAR_BLOCK* local);
void             pBat_FreeLocalBlock(LOCAL_VAR_BLOCK* local);

char* pBat_SkipU8Chars(char* ch, size_t cnt);
size_t pBat_CountU8Chars(const char* ch);

void pBat_GetTimeBasedVar(char type, char* buf, size_t size);
int pBat_TestLocalVarName(char cVar);

#endif
