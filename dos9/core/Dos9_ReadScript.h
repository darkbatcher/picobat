/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2017 Romain GARBI
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
 */

#ifndef DOS9_READSCRIPT_H
#define DOS9_READSCRIPT_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <libDos9.h>
#include <wchar.h>

#ifdef WIN32
#include <windows.h>
#endif

/*  These two structures are designed to store batch command line in a
    time-effective manner. Basically the structure cmdlines_t is a
    structure that is designed to store command lines in order
    to prevent them to from being red over and over again from the file
    if the script contains loops for example.

    By the way, neither blank lines, comments nor labels are to be stored
    in this, saving up speed.

    The labels_t is a linked list that enumerate all the existing labels
    and provide pointers to the line following the specified labels

 */

struct cmdlines_t {
    char* line; /* the line to be parsed */
    struct cmdlines_t* next; /* pointer to next line */
};

struct labels_t {
    char* label; /* name of the label */
    struct cmdlines_t* following; /* line preceding the label */
    struct labels_t* next; /* next label */
};

struct batch_script_t {
    char name[FILENAME_MAX]; /* name of the batch script */
#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
    wchar_t *wname; /* wide character version of wname to speed script up  */
#endif
    int cpy; /* if this struct is a copy (i.e. not the original one) */
    size_t size; /* last modified time */
    struct cmdlines_t* cmds; /* start of the script */
    struct cmdlines_t* curr; /* current line */
    struct labels_t* lbls; /* start of label list */
};

/* Open a batch script */
int Dos9_OpenBatchScript(struct batch_script_t* script, char* name);

/* Reload a batch script */
int Dos9_ReloadBatchScript(struct batch_script_t* script);

/* Load a batch script */
int Dos9_LoadBatchScript(struct batch_script_t* script);

/* Check batch script changes */
int Dos9_CheckBatchScriptChanges(struct batch_script_t* script);

/* Free structures within the batch_script_t struct */
void Dos9_FreeBatchScript(struct batch_script_t* script);

/* Duplicate batch script */
void Dos9_CopyBatchScript(struct batch_script_t* recv,
                                    struct batch_script_t* orig);

/* Free labels */
void Dos9_FreeLabels(struct labels_t* labels);

/* Free cmdlines */
void Dos9_FreeCmdlines(struct cmdlines_t* cmdlines);

size_t Dos9_GetScriptSize(struct batch_script_t* script, int* restrict err);

char* xstrdup(const char* str);

#endif // DOS9_READSCRIPT_H
