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

#ifndef PBAT_MORE_H
#define PBAT_MORE_H

int pBat_CmdMore(char* lpLine);

#define PBAT_MORE_CLEAR     (0x01) /* Clear the screen at every new page */
#define PBAT_MORE_SQUEEZE   (0x02) /* Squeeze blank lines */
#define PBAT_MORE_FORMFEED  (0x04) /* Don't know the actual effect on microsoft
                                      windows, so does nothing */
#define PBAT_MORE_USEU8     (0x08) /* Enable internal utf-8 support */
#define PBAT_MORE_ANSICODES (0x10) /* Enable ansi-codes clever handling */
#define PBAT_MORE_INTERACTIVE (0x20) /* interractive interactive */

int pBat_MoreFile(FILE* in, int flags, int tabsize, int begin, char* filename);

#endif /* PBAT_MORE_H */
