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

#ifndef GLOBAL_H
#define GLOBAL_H

#define	YYSTYPE char*

extern char* IntEval_String;
extern int   IntEval_Result;
extern int	 IntEval_Error;
extern int (*IntEval_Get)(const char*);
extern int (*IntEval_Set)(const char*, int);

void IntEval_InputReset();

void IntEval_FreeStrings(void);
char* IntEval_MakeString(int nb);
int   IntEval_GetValue(const char* str);

int   IntEval_parse(void);
int   IntEval_lex(void);
int   IntEval_error(char *s);

struct string_ll_t {
    char* str;
    struct string_ll_t* next;
};

extern struct string_ll_t* IntEval_Strings;

#define INTEVAL_NOERROR 0
#define INTEVAL_ALLOCERROR 1
#define INTEVAL_SYNTAXERROR 2

#endif // GLOBAL_H
