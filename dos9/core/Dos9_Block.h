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
 */

#ifndef DOS9_BLOCK_H
#define DOS9_BLOCK_H

/* Note that both following functions are inter-dependent.
   It's basically a binary recursion, ie :

    Dos9_GetNextBlockEnd -> Dos9_GetBlockLineEnd
      -> Dos9_GetNextBlockEnd ...

   and so on and so forth.
*/


 /* Gets the end of block
    if lpCh does not point to a '(' character, the return value is NULL
    if not end can be found the return value is NULL
 */
char* Dos9_GetNextBlockEnd(char* lpCh);

/* Get the end of a line, taking account of multiples
   blocks on a single line */
char* Dos9_GetBlockLineEnd(char* lpCh);

/* get the begining of the very first next block*/
char* Dos9_GetNextBlockBegin(char* lpCh);
char* Dos9_GetNextBlockBeginEx(char* lpCh, int bIsBlockCmd);

 #endif // DOS9_BLOCK_H
