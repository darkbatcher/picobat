/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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

#include <libpBat.h>

#include "../core/pBat_Core.h"

#include "pBat_Commands.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"
#include "pBat_IfExp.h"

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

char * pBat_IfExp_Compute(char* line, int* ret, int flags)
{
    ifexp_t* exp;
    ifexp_line_t* lineexp, *linetmp;

    line = pBat_IfExp_ParseLine(line, &lineexp);

    if (lineexp == NULL){

        *ret = -1; /* report error */
        return line;

    }

    linetmp = lineexp;

    exp = pBat_IfExp_Parse(lineexp);

    if (exp == NULL) {

        *ret = -1; /* report error */
        return line;

    }

    *ret = pBat_IfExp_Evaluate(exp, flags);

    /* printf("Returning \"%s\" and %d\n", line, *ret); */

    pBat_IfExp_Free(exp);

    return line;

}


int pBat_IfExp_Evaluate(ifexp_t* exp, int flags)
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
        return pBat_IfExp_ExecuteTest(exp->child1.child.ops, flags);

    }

    /* printf("\t-child2.type = %d\n", exp->child2.type);
    printf("\t-child2.child = %d\n", exp->child2.child);
    if (exp->child1.type == IFEXP_NODE_OPS)
        _Dump_Line_t(exp->child2.child.ops);
    printf("done \n\n"); */

    if (exp->child1.type == IFEXP_NODE_OPS) {

        res1 = pBat_IfExp_ExecuteTest(exp->child1.child.ops, flags);

    } else {

        res1 = pBat_IfExp_Evaluate(exp->child1.child.exp, flags);

    }

    if (res1 == -1)
        return -1;

    /* take a shortcut here, on some occasions you don't have to
       evaluate the left hand side of the expression */
    if (exp->type == IFEXP_AND )
        if (!res1)
            return 0;
    else if (res2)
        return 1;

    if (exp->child2.type == IFEXP_NODE_OPS) {

        res2 = pBat_IfExp_ExecuteTest(exp->child2.child.ops, flags);

    } else {

        res2 = pBat_IfExp_Evaluate(exp->child2.child.exp, flags);

    }

    if (res2 == -1)
        return -1;

    if (exp->type == IFEXP_AND) {

        return res1 && res2;

    } else {

        return res1 || res2;

    }
}

int pBat_IfExp_ExecuteTest(ifexp_line_t* test, int flags)
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
        PBAT_IF_DEFINED_TEST(result, test->op->str, lpeEnv);

    } else if (!stricmp(test->op->str, "EXIST")) {

        NEXT_ELEMENT(test);

        PBAT_IF_EXIST_TEST(result, test->op->str, files);

    } else if (!stricmp(test->op->str,  "ERRORLEVEL")) {

        NEXT_ELEMENT(test);
        PBAT_IF_ERRORLEVEL_TEST(result, test->op->str, lpeEnv);

    } else {

        array[0] = test->op->str;
        NEXT_ELEMENT(test);

        array[1] = test->op->str;
        NEXT_ELEMENT(test);

        if ((result = pBat_PerformExtendedTest(array[1],
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


ifexp_t* pBat_IfExp_Parse(ifexp_line_t* line)
{
    ifexp_line_t * rhs;
    ifexp_t* exp;


    pBat_IfExp_SuppressBrackets(&line);

    if (line == NULL)
        return NULL;

    if (!(exp = malloc(sizeof(ifexp_t))))
        return NULL;

    if (pBat_IfExp_Cut("AND", &line, &rhs)) {

        if (line == NULL || rhs == NULL)
            goto error;

        exp->type = IFEXP_AND;

        pBat_IfExp_FillChild(&(exp->child1), line);
        pBat_IfExp_FillChild(&(exp->child2), rhs);


    } else if (pBat_IfExp_Cut("OR", &line, &rhs)) {

        if (line == NULL || rhs == NULL)
            goto error;

        exp->type = IFEXP_OR;

        pBat_IfExp_FillChild(&(exp->child1), line);
        pBat_IfExp_FillChild(&(exp->child2), rhs);

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

void pBat_IfExp_FillChild(ifexp_child_t* child, ifexp_line_t* line)
{

    pBat_IfExp_SuppressBrackets(&line);

    if ((child->child.exp = pBat_IfExp_Parse(line))) {

        child->type = IFEXP_NODE_EXP;

    } else {

        child->child.ops = line;
        child->type = IFEXP_NODE_OPS;

    }

}

void pBat_IfExp_SuppressBrackets(ifexp_line_t** line)
{
    while (pBat_IfExp_SuppressOneBracketPair(line));
    /*_Dump_Line_t(*line);*/
}

int pBat_IfExp_SuppressOneBracketPair(ifexp_line_t** line)
{
    int count = 1;

    ifexp_line_t *ops = *line,
        *prev;

    if (!ops || !EQUCHR(ops->op->str, '['))
        return 0;

    prev = ops;

    while (ops->next) {

        prev = ops;
        ops = ops->next;


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
    pBat_EsFree(ops->op);
    free(ops);

    /* free the first bracket */
    ops = *line;
    *line = ops->next;
    pBat_EsFree(ops->op);
    free(ops);

    return 1;
}

int pBat_IfExp_Cut(char* tok, ifexp_line_t** begin, ifexp_line_t** end)
{
    ifexp_line_t * line = *begin;
    int level = 0;

    if (line && !stricmp(line->op->str, tok)) {

        *begin = NULL;
        *end = line->next;

        pBat_EsFree(line->op);
        free (line);

    }

    if (EQUCHR(line->op->str, '['))
        level = 1;

    while (line->next) {

        if (EQUCHR(line->next->op->str, '[')) {
            level ++;
        } else if (EQUCHR(line->next->op->str, ']')) {
            level --;
        } else if ((level == 0)
                && !stricmp(line->next->op->str, tok)) {

            /* the token was found */

            *end = line->next->next;
            pBat_EsFree(line->next->op);
            free(line->next);
            line->next = NULL;

            return 1;

        }

        line = line->next;

    }

    return 0;
}

char * pBat_IfExp_ParseLine(char* line, ifexp_line_t** ops)
{
    int level = 0;
    ESTR* str = pBat_EsInit();
    ifexp_line_t *ret = NULL, *last=NULL;

    while ((line = pBat_GetNextParameterEs(line, str))) {

        if (!strcmp("[", pBat_EsToChar(str))) {

            level ++;

        } else if (!strcmp("]", pBat_EsToChar(str))) {

            level --;

        }

        if (pBat_IfExp_AddLineElement(str, &ret, &last)) {
            pBat_IfExp_FreeLine(ret);
            return NULL;
        }

        if (level <= 0) {
            break;
        }

        str = pBat_EsInit();

    }

    *ops = ret;

    return line;
}

int pBat_IfExp_AddLineElement(ESTR* elem, ifexp_line_t** line, ifexp_line_t** ops)
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

void pBat_IfExp_Free(ifexp_t* exp)
{

    if (exp->type == IFEXP_NONE )  {

        pBat_IfExp_FreeLine(exp->child1.child.ops);

    } else {

        switch (exp->child1.type) {

            case IFEXP_NODE_EXP :
                pBat_IfExp_Free(exp->child1.child.exp);
                break;

            default:
                pBat_IfExp_FreeLine(exp->child1.child.ops);

        }

        switch (exp->child2.type) {

            case IFEXP_NODE_EXP :
                pBat_IfExp_Free(exp->child2.child.exp);
                break;

            default:
                pBat_IfExp_FreeLine(exp->child2.child.ops);

        }

    }

    free(exp);

}

void pBat_IfExp_FreeLine(ifexp_line_t* line)
{
    ifexp_line_t* next;

    while (line) {

        next = line->next;

        pBat_EsFree(line->op);
        free(line);

        line = next;

    }

}
