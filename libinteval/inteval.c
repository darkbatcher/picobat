/*
 *
 *   libinteval, a lib for evaluating integer expression
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
#include <string.h>
#include <stdlib.h>

#include "inteval.h"
#include "global.h"

int IntEval_Error=INTEVAL_NOERROR;
int(*IntEval_Get)(const char*);
int(*IntEval_Set)(const char*, int);

int IntEval_Set_Fn(int(*get)(const char*),
                        int(*set)(const char*, int))
{
    IntEval_Get = get;
    IntEval_Set = set;
    return 0;
}

int IntEval_Eval(const char* lpLine)
{
    char* lpInteval;

	if (!(IntEval_String = strdup(lpLine))) {

		IntEval_Error=INTEVAL_ALLOCERROR;
        return 0;

    }


	lpInteval=IntEval_String;

	IntEval_InputReset();
	IntEval_Error=INTEVAL_NOERROR;

	IntEval_Result=0;
	IntEval_Strings = NULL;

    IntEval_parse();

    IntEval_FreeStrings();

	free(lpInteval);

    return IntEval_Result;
}

char* IntEval_MakeString(int nb)
{
    char buf[FILENAME_MAX];
    struct string_ll_t* item;

    if (!(item = malloc (sizeof (struct string_ll_t))))
        return "0";

    snprintf(buf, sizeof(buf), "%d", nb);

    if (!(item->str = strdup(buf))) {
        free(item);
        return "0";
    }

    item->next = IntEval_Strings;
    IntEval_Strings = item;
    return item->str;
}

void IntEval_FreeStrings(void)
{
    struct string_ll_t* item;

    while (IntEval_Strings != NULL) {
        item = IntEval_Strings->next;
        free(IntEval_Strings->str);
        free(IntEval_Strings);
        IntEval_Strings = item;
    }
}

int   IntEval_GetValue(const char* str)
{
    int ret;
    char *pch;

    if ((pch = strchr(str, '+'))
        || (pch = strchr(str, '-'))
        || (pch = strchr(str, '*'))
        || (pch = strchr(str, '/'))
        || (pch = strchr(str, '|'))
        || (pch = strchr(str, '^'))
        || (pch = strchr(str, '>'))
        || (pch = strchr(str, '<'))
        || (pch = strchr(str, '&')))
        *pch = '\0';

    ret = strtol(str, &pch, 0);
    if (pch && *pch != '\0')
        ret = IntEval_Get(str);

    return ret;
}
