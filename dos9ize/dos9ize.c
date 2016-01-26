/*
 *
 *   dos9ize, a free Dos9 batch adapter, The Dos9 Project
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
#include <stdio.h>
#include <stdlib.h>

#include <libDos9.h>

#include "dos9ize.h"
/* dos9ize file [output]

    Do some adjustments to batch files to make them running using Dos9.

    Adjustments performed are :

        - Adding spaces between switches
        - Removing ^^^! in favor of ^!

*/

command commands[] = {
    {"RD", "SQ"},
    {"RMDIR", "S"},
    {"RMDIR", "Q"},
    {NULL, NULL}
};

int main(int argc, char* argv[])
{
    pair p;
    int i = 0;

    ESTR* toreplace=Dos9_EsInit();
    ESTR* replaceby=Dos9_EsInit();

    while (commands[i].cmd != NULL) {

        pair_start(&p, commands+i);

        do {

            pair_replace_exps(&p, toreplace, replaceby);
            printf("replace \"%s\" by \"%s\"\n", toreplace->str, replaceby->str);

        } while (pair_next(&p));

        i++;

    }


    Dos9_EsFree(replaceby);
    Dos9_EsFree(toreplace);

    return 0;
}
