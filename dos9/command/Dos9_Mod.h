/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2018 Romain GARBI
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
#ifndef DOS9_MOD_H
#define DOS9_MOD_H

int Dos9_CmdMod(char* line);

#define DOS9_MOD_SETENV 0
#define DOS9_MOD_GETENV 1
#define DOS9_MOD_GETCURRENTDIR 2
#define DOS9_MOD_SETCURRENTDIR 3
#define DOS9_MOD_GETFINPUT 4
#define DOS9_MOD_GETFOUTPUT 5
#define DOS9_MOD_GETFERROR 6
#define DOS9_MOD_GETBISSCRIPT 7
#define DOS9_MOD_GETNEXTPARAMETERES 8
#define DOS9_MOD_ESTOFULLPATH 9
#define DOS9_MOD_REGISTERCOMMAND 10
#define DOS9_MOD_SHOWERRORMESSAGE 11
#define DOS9_MOD_RUNLINE 12
#define DOS9_MOD_API_VERSION 0

#endif // DOS9_MOD_H
