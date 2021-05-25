/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
#ifndef PBAT_IF_EXP_H
#define PBAT_IF_EXP_H

#include <libpBat.h>

/* if [ exp ] (
        ...
   )

   exp :

   [ exp ]    : subexpression
   exp or exp : or
   exp and exp : and

   [not] defined varname
   [not] exist filename

   var1 cmp var2


   */

#define IFEXP_NONE 0
#define IFEXP_OR 1
#define IFEXP_AND 2

#define IFEXP_NODE_EXP 0
#define IFEXP_NODE_OPS 1

typedef struct ifexp_line_t {
    ESTR* op;
    struct ifexp_line_t* next;
} ifexp_line_t;

typedef struct ifexp_child_t {
    int type;
    union {
        struct ifexp_t* exp;
        ifexp_line_t* ops;
    } child;
} ifexp_child_t;

typedef struct ifexp_t {
    int type;
    ifexp_child_t child1;
    ifexp_child_t child2;
} ifexp_t;

char * pBat_IfExp_Compute(char* line, int* ret, int flags);
int pBat_IfExp_Evaluate(ifexp_t* exp, int flags);
int pBat_IfExp_ExecuteTest(ifexp_line_t* test, int flags);
ifexp_t* pBat_IfExp_Parse(ifexp_line_t* line);
void pBat_IfExp_FillChild(ifexp_child_t* child, ifexp_line_t* line);
void pBat_IfExp_SuppressBrackets(ifexp_line_t** line);
int pBat_IfExp_SuppressOneBracketPair(ifexp_line_t** line);
int pBat_IfExp_Cut(char* tok, ifexp_line_t** begin, ifexp_line_t** end);
char * pBat_IfExp_ParseLine(char* line, ifexp_line_t** ops);
int pBat_IfExp_AddLineElement(ESTR* elem, ifexp_line_t** line, ifexp_line_t** ops);
void pBat_IfExp_Free(ifexp_t* exp);
void pBat_IfExp_FreeLine(ifexp_line_t* line);

#endif
