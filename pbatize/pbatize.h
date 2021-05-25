/*
 *
 *   pbatize, a free pBat batch adapter, The pBat Project
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
#ifndef PBATISE_H
#define PBATISE_H

#define SWITCHES_SIZE 32

#include <libpBat.h>

typedef struct command {
    const char* const cmd;
    const char* const switches;
} command;

typedef struct pair {
    command* cmd;
    int i;
    int j;
} pair; /*a pair of either command and switches or a pair of switches */

void pair_start(pair* p, command* cmd);
int pair_next(pair* p);
void pair_regexp(pair* p, ESTR* regexp);
void pair_replace_exps(pair* p, ESTR* toreplace, ESTR* replaceby);

#endif // PBATISE_H
