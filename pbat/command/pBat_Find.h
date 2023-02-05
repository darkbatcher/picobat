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

#ifndef PBAT_FIND_H
#define PBAT_FIND_H

#define PBAT_FIND_CASE_UNSENSITIVE 1
#define PBAT_FIND_CASE_SENSITIVE   0
#define PBAT_FIND_TRADITIONAL      0
#define PBAT_FIND_REGEXP           0x02

int pBat_CmdFind(char* lpLine);

#endif /* PBAT_FIND_H */
