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

#ifndef DOS9_MORE_H
#define DOS9_MORE_H

int Dos9_CmdMore(char* lpLine);

#define DOS9_MORE_CLEAR     (0x01) /* Clear the screen at every new page */
#define DOS9_MORE_SQUEEZE   (0x02) /* Squeeze blank lines */
#define DOS9_MORE_FORMFEED  (0x04) /* Don't know the actual effect on microsoft
                                      windows, so does nothing */
#define DOS9_MORE_USEU8     (0x08) /* Enable internal utf-8 support */
#define DOS9_MORE_ANSICODES (0x10) /* Enable ansi-codes clever handling */

int Dos9_MoreFile(int flags, int tabsize, int begin, char* filename);

#endif /* DOS9_MORE_H */
