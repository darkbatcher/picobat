/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2016 Romain GARBI
 *   Copyright (C) 2017      Teddy ASTIE
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
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Commands.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"
#include "Dos9_IfExp.h"

#define MIN(x,y) (x > y) ? (y) : (x)
#define NEGATE(nb, state) (state) ? (!nb) : (nb)
#define EQUCHR(str, chr) (*(str) == (chr) && *((str)+1) == '\0')
#define NEXT_ELEMENT(line) \
    if (!(line = line->next)) return -1;

void  _Dump_Line_t(ifexp_line_t* line)
{

    printf("{");

    while (line) {

        printf("\"%s\",", line->op->str);

        line = line->next;

    }
    printf("}\n");

}

char * Dos9_IfExp_Compute(const char* line, int* ret, int flags)
{
    ifexp_t* exp;
    ifexp_line_t* lineexp, *linetmp;

    line = Dos9_IfExp_ParseLine(line, &lineexp);

    if (lineexp == NULL)
        return NULL;

    linetmp = lineexp;

    exp = Dos9_IfExp_Parse(lineexp);

    *ret = Dos9_IfExp_Evaluate(exp, flags);

    /* printf("Returning \"%s\" and %d\n", line, *ret); */

    Dos9_IfExp_Free(exp);

    return line;

}


int Dos9_IfExp_Evaluate(ifexp_t* exp, int flags)
{
    int res1, res2;

    /* printf("Evaluating exp at %X\n", exp);
    printf("\t-type = %d\n", exp->type);
    printf("\t-child1.type = %d\n", exp->child1.type);
    printf("\t-child1.child = %d\n", exp->child1.child);
    if (exp->child1.type == IFEXP_NODE_OPS)
        _Dump_Line_t(exp->child1.child.ops); */


    if (exp->type == IFEXP_NONE) {

        /* printf("done \n\n"); */
        return Dos9_IfExp_ExecuteTest(exp->child1.child.ops, flags);

    }

    /* printf("\t-child2.type = %d\n", exp->child2.type);
    printf("\t-child2.child = %d\n", exp->child2.child);
    if (exp->child1.type == IFEXP_NODE_OPS)
        _Dump_Line_t(exp->child2.child.ops);
    printf("done \n\n"); */

    if (exp->child1.type == IFEXP_NODE_OPS) {

        res1 = Dos9_IfExp_ExecuteTest(exp->child1.child.ops, flags);

    } else {

        res1 = Dos9_IfExp_Evaluate(exp->child1.child.exp, flags);

    }

    if (res1 == -1)
        return -1;

    if (exp->child2.type == IFEXP_NODE_OPS) {

        res2 = Dos9_IfExp_ExecuteTest(exp->child2.child.ops, flags);

    } else {

        res2 = Dos9_IfExp_Evaluate(exp->child2.child.exp, flags);

    }

    if (res2 == -1)
        return -1;

    if (exp->type == IFEXP_AND) {

        return res1 && res2;

    } else {

        return res1 || res2;

    }
}

int Dos9_IfExp_ExecuteTest(ifexp_line_t* test, int flags)
{
    int neg = 0,
        result;
    char*  array[2] = {NULL, NULL};

    FILELIST* files;

    if (!stricmp(test->op->str, "NOT")) {

        neg = 1;

        if (!(test = test->next))
            return -1; /* return an error */

    }

    if (!stricmp(test->op->str, "DEFINED")) {

        NEXT_ELEMENT(test);
        DOS9_IF_DEFINED_TEST(result, test->op->str, lpeEnv);

    } else if (!stricmp(test->op->str, "EXIST")) {

        NEXT_ELEMENT(test);

        DOS9_IF_EXIST_TEST(result, test->op->str, files);

    } else if (!stricmp(test->op->str,  "ERRORLEVEL")) {

        NEXT_ELEMENT(test);
        DOS9_IF_ERRORLEVEL_TEST(result, test->op->str, lpeEnv);

    } else {

        array[0] = test->op->str;
        NEXT_ELEMENT(test);

        array[1] = test->op->str;
        NEXT_ELEMENT(test);

        if ((result = Dos9_PerformExtendedTest(array[1],
                                                array[0],
                                                test->op->str,
                                                flags)) == -1)
            return -1;

    }

    if (test->next)
        return -1;

    result = NEGATE(result, neg);

    return result;

}


ifexp_t* Dos9_IfExp_Parse(ifexp_line_t* line)
{
    ifexp_line_t * rhs;
    ifexp_t* exp;

    if (!(exp = malloc(sizeof(ifexp_t))))
        return NULL;


    Dos9_IfExp_SuppressBrackets(&line);

    if (Dos9_IfExp_Cut("AND", &line, &rhs)) {

        if (line == NULL || rhs == NULL)
            goto error;

        exp->type = IFEXP_AND;

        Dos9_IfExp_FillChild(&(exp->child1), line);
        Dos9_IfExp_FillChild(&(exp->child2), rhs);


    } else if (Dos9_IfExp_Cut("OR", &line, &rhs)) {

        if (line == NULL || rhs == NULL)
            goto error;

        exp->type = IFEXP_OR;

        Dos9_IfExp_FillChild(&(exp->child1), line);
        Dos9_IfExp_FillChild(&(exp->child2), rhs);

    } else {

        exp->type = IFEXP_NONE;
        exp->child1.type = IFEXP_NODE_OPS;
        exp->child1.child.ops = line;

    }

    return exp;

error:
    free(exp);
    return NULL;
}

void Dos9_IfExp_FillChild(ifexp_child_t* child, ifexp_line_t* line)
{

    Dos9_IfExp_SuppressBrackets(&line);

    if (child->child.exp = Dos9_IfExp_Parse(line)) {

        child->type = IFEXP_NODE_EXP;

    } else {

        child->child.ops = line;
        child->type = IFEXP_NODE_OPS;

    }

}

void Dos9_IfExp_SuppressBrackets(ifexp_line_t** line)
{
    while (Dos9_IfExp_SuppressOneBracketPair(line));
    /*_Dump_Line_t(*line);*/
}

int Dos9_IfExp_SuppressOneBracketPair(ifexp_line_t** line)
{
    int count = 1;

    ifexp_line_t *ops = *line,
        *prev;

    if (!ops || !EQUCHR(ops->op->str, '['))
        return 0;

    /* printf("Start = ");
    _Dump_Line_t(*line); */

    prev = ops;

    while (ops->next) {

        prev = ops;
        ops = ops->next;

        /* printf("Count : %d\tstr : \"%s\" \t ops->next : %X\n", count, ops->op->str, ops->next); */


        if (EQUCHR(ops->op->str, '[')) {

            count ++;

        } else if (EQUCHR(ops->op->str, ']') && count > 0) {

            count --;

        }

        /* if count once reach 0, it's impossible to consider
           that expression is surrounded by brackets, unless it's the
           last element  */
        if (count == 0 && ops->next != NULL)
            return 0;

    }


    /* free the last bracket */
    prev->next = NULL;
    Dos9_EsFree(ops->op);
    free(ops);

    /* free the first bracket */
    ops = *line;
    *line = ops->next;
    Dos9_EsFree(ops->op);
    free(ops);

    /* printf("Result = ");
    _Dump_Line_t(*line); */

    return 1;
}

int Dos9_IfExp_Cut(const char* tok, ifexp_line_t** begin, ifexp_line_t** end)
{
    ifexp_line_t * line = *begin;
    int level = 0;

    if (line && !stricmp(line->op->str, tok)) {

        *begin = NULL;
        *end = line->next;

        Dos9_EsFree(line->op);
        free (line);

    }

    if (EQUCHR(line->op->str, '['))
        level = 1;

    while (line->next) {

        /* printf("tok = \"%s\"\tlevel = %d\tNext = %X\n", line->next->op->str, level, line->next->next); */

        if (EQUCHR(line->next->op->str, '[')) {
            level ++;
        } else if (EQUCHR(line->next->op->str, ']')) {
            level --;
        } else if ((level == 0)
                && !stricmp(line->next->op->str, tok)) {

            /*printf("\tMatching token found\n");*/

            /* the token was found */

            *end = line->next->next;
            Dos9_EsFree(line->next->op);
            free(line->next);
            line->next = NULL;

            return 1;

        }

        line = line->next;

    }

    return 0;
}

char * Dos9_IfExp_ParseLine(const char* line, ifexp_line_t** ops)
{
    int level = 0;
    ESTR* str = Dos9_EsInit();
    ifexp_line_t *ret = NULL, *last=NULL;

    while ((line = Dos9_GetNextParameterEs(line, str))) {

        if (!strcmp("[", Dos9_EsToChar(str))) {

            level ++;

        } else if (!strcmp("]", Dos9_EsToChar(str))) {

            level --;

        }

        if (Dos9_IfExp_AddLineElement(str, &ret, &last)) {
            Dos9_IfExp_FreeLine(ret);
            return NULL;
        }

        if (level <= 0) {
            break;
        }

        str = Dos9_EsInit();

    }

    *ops = ret;

    return line;
}

int Dos9_IfExp_AddLineElement(ESTR* elem, ifexp_line_t** line, ifexp_line_t** ops)
{
    ifexp_line_t* new = malloc(sizeof(ifexp_line_t));

    if (new == NULL)
        return 1;

    if (*line == NULL)
        *line = new;

    if (*ops != NULL)
        (*ops)->next = new;

    *ops = new;

    new->op = elem;
    new->next = NULL;

    return 0;
}

void Dos9_IfExp_Free(ifexp_t* exp)
{

    if (exp->type == IFEXP_NONE )  {

        Dos9_IfExp_FreeLine(exp->child1.child.ops);

    } else {

        switch (exp->child1.type) {

            case IFEXP_NODE_EXP :
                Dos9_IfExp_Free(exp->child1.child.exp);
                break;

            default:
                Dos9_IfExp_FreeLine(exp->child1.child.ops);

        }

        switch (exp->child2.type) {

            case IFEXP_NODE_EXP :
                Dos9_IfExp_Free(exp->child2.child.exp);
                break;

            default:
                Dos9_IfExp_FreeLine(exp->child2.child.ops);

        }

    }

    free(exp);

}

void Dos9_IfExp_FreeLine(ifexp_line_t* line)
{
    ifexp_line_t* next;

    while (line) {

        next = line->next;

        Dos9_EsFree(line->op);
        free(line);

        line = next;

    }

}
