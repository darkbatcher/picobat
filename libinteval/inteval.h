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
 #ifndef INTEVAL_H
#define INTEVAL_H

#define INTEVAL_NOERROR 0
#define INTEVAL_ALLOCERROR 1
#define INTEVAL_SYNTAXERROR 2

int IntEval_Eval(const char* lpLine);
int IntEval_Set_Fn(int(*get)(const char*),
                        int(*set)(const char*, int));

extern int IntEval_Error;

#endif // INTEVAL_H

