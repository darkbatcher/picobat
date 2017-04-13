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
#include "Dos9_Core.h"
#include "../errors/Dos9_Errors.h"

#include <errno.h>
#include <string.h>

/* Jump to a special position in file

    returns -1 in case of error
    return 0 if the jump is achieved

 */
int Dos9_JumpToLabel(char* lpLabelName, char* lpFileName)
{
    struct labels_t* lbl;
    struct batch_script_t script;
    size_t size = strlen(lpLabelName);
    int new;

	if (lpFileName && stricmp(ifIn.batch.name, lpFileName)) {

        if (Dos9_OpenBatchScript(&script, lpFileName)) {

            Dos9_FreeBatchScript(&script);
            return -1;
        }

        lbl = script.lbls;
        new = 1;

	} else {

        lbl = ifIn.batch.lbls;
        new = 0;

	}

    while (lbl && (strnicmp(lbl->label->str, lpLabelName, size)
                        && !Dos9_IsDelim(*(lbl->label->str + size))))
        lbl = lbl->next;

    if (lbl == NULL) {

        Dos9_FreeBatchScript(&script);
        return -1;

    }

    /* a label has been found */
    if (new) {

        /* Set the new file as the new file */
        Dos9_FreeBatchScript(&(ifIn.batch));
        memcpy(&(ifIn.batch), &script, sizeof(struct batch_script_t));

    }

    /* Check if lbl follow a line or is the first line */
    ifIn.batch.curr = lbl->following ? lbl->following->next : ifIn.batch.cmds;
	return 0;
}

