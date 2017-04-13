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

#ifndef DOS9_JUMP_H_INCLUDED
#define DOS9_JUMP_H_INCLUDED


/* automaticly jump to a label
    - lpLabelName : the label name include ':'
    - lpFileName : (Optional) the file to search into */
int Dos9_JumpToLabel(char* lpLabelName, char* lpFileName);
int Dos9_JumpToLabel_Cmdly(char* lpLabelName, char* lpFileName);


#endif // DOS9_JUMP_H_INCLUDED
