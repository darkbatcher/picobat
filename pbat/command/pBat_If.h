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

#ifndef PBAT_CMD_IF_H
#define PBAT_CMD_IF_H

#include "../core/pBat_Core.h"

#define PBAT_IF_CASE_UNSENSITIVE 1
#define PBAT_IF_NEGATION 2
#define PBAT_IF_EXIST 4
#define PBAT_IF_ERRORLEVEL 8
#define PBAT_IF_DEFINED 16

#define PBAT_FLOAT_EQUAL_PRECISION 0.0000000000001

#define CMPTYPE int
#define CMP_EQUAL 1
#define CMP_DIFFERENT 2
#define CMP_GREATER 3
#define CMP_GREATER_OR_EQUAL 4
#define CMP_LESSER 5
#define CMP_LESSER_OR_EQUAL 6
#define CMP_FLOAT_COMP 0x10

#define PBAT_IF_EXIST_TEST(ret, file, filelist) \
    char _if_exist_test_name[FILENAME_MAX]; \
    pBat_MakeFullPath(_if_exist_test_name, file, FILENAME_MAX); \
    filelist = pBat_GetMatchFileList(_if_exist_test_name, \
                                                PBAT_SEARCH_GET_FIRST_MATCH \
                                                | PBAT_SEARCH_NO_STAT \
                                                | PBAT_SEARCH_NO_PSEUDO_DIR); \
    ret=(filelist != NULL); \
    if (filelist) \
        pBat_FreeFileList(filelist);

#define PBAT_IF_DEFINED_TEST(ret, var, env) \
    ret = (pBat_GetEnv(env, var) != NULL);

#define PBAT_IF_ERRORLEVEL_TEST(ret, val, env) \
    ret = !stricmp(val, pBat_GetEnv(env, "ERRORLEVEL"));

int pBat_PerformExtendedTest(const char* lpCmp, const char* lpParam1, const char* lpParam2, int iFlag);

int pBat_CmdIf(char* lpParam, PARSED_LINE** lpplLine); // function to support if

#endif // PBAT_CMD_CONDITION_H
