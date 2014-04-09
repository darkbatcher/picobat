/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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

#ifndef DOS9_LANG_INCLUDED_H
#define DOS9_LANG_INCLUDED_H



extern const char* lpMsgEchoOn;
extern const char* lpMsgEchoOff;
extern const char* lpMsgPause;

extern const char* lpDirNoFileFound;
extern const char* lpDirListTitle;
extern const char* lpDirFileDirNb;


extern const char* lpHlpMain;
extern const char* lpHlpDeprecated;

void Dos9_LoadStrings(void); /* a function that loads differents
                               languages traductions */

#endif
