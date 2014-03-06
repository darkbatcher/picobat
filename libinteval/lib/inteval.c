/*
 *
 *   libinteval, a lib for evaluating integer expression
 *   Copyright (C) 2010-2014 DarkBatcher
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

    IntEval_parse();

	free(lpInteval);

    return IntEval_Result;
}
