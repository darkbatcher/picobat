/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2013 DarkBatcher
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

#ifndef DOS9_CMD_IF_H
#define DOS9_CMD_IF_H

#define DOS9_IF_CASE_UNSENSITIVE 1
#define DOS9_IF_NEGATION 2
#define DOS9_IF_EXIST 4
#define DOS9_IF_ERRORLEVEL 8
#define DOS9_IF_DEFINED 16

#define DOS9_FLOAT_EQUAL_PRECISION 0.0000000000001

typedef enum CMPTYPE {
    CMP_EQUAL,
    CMP_DIFFERENT,
    CMP_GREATER,
    CMP_GREATER_OR_EQUAL,
    CMP_LESSER,
    CMP_LESSER_OR_EQUAL,
    CMP_FLOAT_EQUAL
} CMPTYPE;

int Dos9_CmdIf(char* lpParam); // function to support if

#endif // DOS9_CMD_CONDITION_H
