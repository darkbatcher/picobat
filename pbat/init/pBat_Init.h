/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2014 Romain GARBI
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

#ifndef PBAT_INIT_INCLUDED_H
#define PBAT_INIT_INCLUDED_H

void pBat_AssignCommandLine(int c, char** argv);
void pBat_Init();
char* pBat_GetParameters(char** argv, char** lpFileName, int* bExitAfterCmd, int*
                            bQuiet);
void pBat_InitConsoleTitles(char *lpFileName, int bQuiet);
void pBat_RunAutoBat(void);
void pBat_DuplicateStdStreams(void);

#endif
