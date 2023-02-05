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
#include <stdio.h>
#include <stdlib.h>

#include <libpBat.h>

#include "pbatize.h"

void pair_start(pair* p, command* cmd)
{
    p->cmd = cmd;
    p->i = 0;
    p->j = -1;
}

int pair_next(pair* p)
{
    if (p->j == -1) {

        /* try command + switch pair */
        if (p->cmd->switches[ ++ (p->i)] == '\0') {

            p->j = 1;

            if (p->i == 1)
                return 0;

            p->i = 0;

        }

    } else {

        if ((p->j + 1) == p->i)
            p->j ++;

        if (p->cmd->switches[++ (p->j)] == '\0' ) {

            p->j = 0;

            if (p->cmd->switches[++ (p->i)] == '\0')
                    return 0;

        }

    }

    return 1;
}

void pair_regexp(pair* p, ESTR* regexp)
{
    char switches[3]="/x";

    pBat_EsCpy(regexp, "*");

    if (p->j == -1) {
        pBat_EsCat(regexp, p->cmd->cmd);
    } else {
        switches[1] = p->cmd->switches[p->j];
        pBat_EsCat(regexp, switches);
    }
    switches[1] = p->cmd->switches[p->i];
    pBat_EsCat(regexp, switches);
    pBat_EsCat(regexp, "*");
}

/* Get the expression to replace and to be replaced in order to correct the use
 * of switches
 */
void pair_replace_exps(pair* p, ESTR* toreplace, ESTR* replaceby)
{
    char switches[3]="/x";

    if (p-> j == -1) {

        pBat_EsCpy(toreplace, p->cmd->cmd);
        pBat_EsCpy(replaceby, p->cmd->cmd);

    } else {

        switches[1] = p->cmd->switches[p->j];
        pBat_EsCpy(toreplace, switches);
        pBat_EsCpy(replaceby, switches);

    }

    pBat_EsCat(replaceby, " ");
    switches[1] = p->cmd->switches[p->i];
    pBat_EsCat(replaceby, switches);
    pBat_EsCat(toreplace, switches);
}
