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

#ifndef PBAT_HELP_H

#define PBAT_HELP_ECHO      0
#define PBAT_HELP_SET       1
#define PBAT_HELP_GOTO      2
#define PBAT_HELP_IF        3
#define PBAT_HELP_CD        4
#define PBAT_HELP_RD        5
#define PBAT_HELP_MD        6
#define PBAT_HELP_DEL       7
#define PBAT_HELP_COPY      8
#define PBAT_HELP_MOVE      9
#define PBAT_HELP_CALL      10
#define PBAT_HELP_FOR       11
#define PBAT_HELP_SETLOCAL  12
#define PBAT_HELP_REM       13
#define PBAT_HELP_DIR       14
#define PBAT_HELP_CLS       16
#define PBAT_HELP_TITLE     17
#define PBAT_HELP_REN       18
#define PBAT_HELP_TYPE      19
#define PBAT_HELP_PAUSE     20
#define PBAT_HELP_COLOR     21
#define PBAT_HELP_EXIT      22
#define PBAT_HELP_DEF     23
#define PBAT_HELP_SHIFT     24
#define PBAT_HELP_FIND      25
#define PBAT_HELP_MORE      26
#define PBAT_HELP_BREAK     27
#define PBAT_HELP_CHCP      28
#define PBAT_HELP_PUSHD     29
#define PBAT_HELP_POPD      30
#define PBAT_HELP_WC        31
#define PBAT_HELP_XARGS     32
#define PBAT_HELP_ENDLOCAL  33
#define PBAT_HELP_PROMPT    34
#define PBAT_HELP_PECHO     35
#define PBAT_HELP_TIMEOUT   36
#define PBAT_HELP_MOD       37
#define PBAT_HELP_VER		38
#define PBAT_HELP_START     39
#define PBAT_HELP_LOCALE    40

#define PBAT_HELP_ARRAY_SIZE 41


/* this is to instanciate a little embedded command
   help, to provide simple syntax help if no other
   help is availiable */
void pBat_LoadInternalHelp(void);
void pBat_ShowInternalHelp(int cmdId);

#endif // PBAT_HELP_H
