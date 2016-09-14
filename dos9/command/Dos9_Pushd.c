/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
 *   Copyright (C) 2016      Astie Teddy
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
#include <string.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Cd.h"
#include "Dos9_Pushd.h"

#include "../errors/Dos9_Errors.h"
#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"


/* Changes the system's current directory and store the previous folder/path for use by the POPD command.

    PUSHD [path]

    Changes the system's current directory and store the previous folder/path for use by the POPD command.

        - [path] : The path for the new current directory.

    The old current directory is stored in a stack.
*/

/* TODO : Makes this more Windows compatible */

static char **stack;
static int stack_pos;

int Dos9_CmdPushd (char *lpLine)
{
    lpLine += 5;
    lpLine = Dos9_SkipBlanks(lpLine);

    char *lpEnd = lpLine + strlen(lpLine) - 1;

    /* Remove ending blanks */
    while (*lpEnd == ' ' || *lpEnd == '\t') {
        *lpEnd = '\0';
        lpEnd--;
    }

    if (!strncmp(lpLine, "/?", 2)) {
        Dos9_ShowInternalHelp(DOS9_HELP_PUSHD);
        return 0;
    }

    if (!strcmp(lpLine, "")) {
        puts(Dos9_GetCurrentDir());
        return 0;
    }

    char *currentDirectory = Dos9_GetCurrentDir();
    char *stackDirectory = malloc(strlen(currentDirectory) * sizeof(char) + 1);
    strcpy(stackDirectory, currentDirectory);
    stackDirectory[strlen(currentDirectory)] = '\0';

    if (Dos9_SetCurrentDir(lpLine)) {
        Dos9_ShowErrorMessage(DOS9_DIRECTORY_ERROR, lpLine, FALSE);
        return -1;
    } else {
        Dos9_CmdPushd_PushDirectory(stackDirectory);
        return 0;
    }
}

void Dos9_CmdPushd_PushDirectory(char *directory);
void Dos9_CmdPushd_PushDirectory(char *directory)
{
    char *newDir = malloc(strlen(directory) * sizeof(char) + 1);
    strcpy(newDir, directory);
    newDir[strlen(directory)] = '\0';

    // Initialize a stack
    if (stack == NULL)
        Dos9_CmdPushd_InitStack();

    // Some issues using reallocation
    // stack = realloc(stack, stack_pos * sizeof(void*) + 1);

    if (stack == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION, "Dos9_Pushd", 1);

    stack[stack_pos + 1] = newDir;
    stack_pos++;
}

void Dos9_CmdPushd_PopDirectory()
{
    if (stack == NULL || stack_pos == 0)
        return;

    free(stack[stack_pos]);

    // Some issues using reallocation
    // stack = realloc(stack, sizeof(void*) * stack_pos);

    if (stack == NULL)
        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION, 'Dos9_Pushd', 1);

    stack_pos--;
}

char* Dos9_CmdPushd_GetDirectory()
{
    if (stack_pos == 0)
        return "";

    return stack[stack_pos];
}

void Dos9_CmdPushd_InitStack();
void Dos9_CmdPushd_InitStack()
{
    stack = malloc(MAX_STACK_SIZE * sizeof(char*));
    stack_pos = 0;
}
