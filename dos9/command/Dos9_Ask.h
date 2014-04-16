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

#ifndef DOS9_ASK_H
#define DOS9_ASK_H

#define DOS9_ASK_YN             (0x00)
/* The use can choose either YES or NO */

#define DOS9_ASK_YNA            (0x08)
/* the user can choose either YES, NO or ALL */

#define DOS9_ASK_DEFAULT_Y      (0x01)
/* if the user input invalid value, the function will
   return DOS9_ASK_YES */

#define DOS9_ASK_DEFAULT_N      (0x02)
/* if the user input invalid value, the function will
return DOS9_ASK_NO */

#define DOS9_ASK_DEFAULT_A      (0x04)
/* if the user input invalid value, the function will
return DOS9_ASK_ALL */

#define DOS9_ASK_INVALID_REASK  (0x10)
/* re-ask if the input turn out to be invalid */

#define DOS9_ASK_YES            (1)
#define DOS9_ASK_NO             (2)
#define DOS9_ASK_ALL            (4)
#define DOS9_ASK_INVALID        (-1)

int Dos9_AskConfirmation(int iFlags, const char* lpMsg, ...);

#endif // DOS9_ASK_H
