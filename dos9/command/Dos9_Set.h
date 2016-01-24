/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifndef DOS9_CMD_SET_H
#define DOS9_CMD_SET_H

#include <libDos9.h>

int Dos9_CmdSet(char *lpLine);
int Dos9_CmdSetS(char* lpLine);
int Dos9_CmdSetP(char* lpLine);
int Dos9_CmdSetA(char* lpLine, int bFloats);
int Dos9_CmdSetEvalFloat(ESTR* lpExpression);
int Dos9_CmdSetEvalInt(ESTR* lpExpression);

#endif // DOS9_CMD_ECHO_H
