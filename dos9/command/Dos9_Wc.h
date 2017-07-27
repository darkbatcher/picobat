/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2017 Romain GARBI
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
#ifndef DOS9_WC_H
#define DOS9_WC_H

#include <stdlib.h>

struct wc_count_t {
    size_t lines;
    size_t words;
    size_t bytes;
    size_t chars;
};

int Dos9_CmdWc(char* line);

#define DOS9_WC_LINES 1
#define DOS9_WC_CHARS 2
#define DOS9_WC_BYTES 4
#define DOS9_WC_WORDS 8

#endif // DOS9_WC_H
