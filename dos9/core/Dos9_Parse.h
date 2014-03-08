/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifndef DOS9_PARSE_H
#define DOS9_PARSE_H

#include <libDos9.h>
#include "Dos9_Core.h"

/* A structure to store command line
 * These information are stored in two different ways:
 * - Normal line : part of a normal line
 * - Blocks of commands : blocks of commands defined by usi braces
 */

struct PARSED_STREAM {
    ESTR* lpCmdLine;
    char cNodeType;
    struct PARSED_STREAM* lppsNode;
};

struct PARSED_STREAM_START {
    struct PARSED_STREAM* lppsStream;
    char cOutputMode;
    char* lpOutputFile;
    char* lpInputFile;
};

typedef struct PARSED_STREAM PARSED_STREAM,*LPPARSED_STREAM;
typedef struct PARSED_STREAM_START PARSED_STREAM_START,*LPPARSED_STREAM_START;

#define PARSED_STREAM_NODE_NONE 0x00
#define PARSED_STREAM_NODE_YES 0x01
#define PARSED_STREAM_NODE_NOT 0x02
#define PARSED_STREAM_NODE_PIPE 0x03
#define PARSED_STREAM_NODE_RESET -1

#define PARSED_STREAM_START_MODE_OUT DOS9_STDOUT
#define PARSED_STREAM_START_MODE_ERROR DOS9_STDERR
#define PARSED_STREAM_START_MODE_TRUNCATE 0x04

PARSED_STREAM_START* Dos9_ParseLine(ESTR* lpLine);

PARSED_STREAM_START* Dos9_ParseOutput(ESTR* lpesLine);
PARSED_STREAM*       Dos9_ParseOperators(ESTR* lpesLine);

void                 Dos9_FreeLine(PARSED_STREAM_START* lpssStreamStart);

PARSED_STREAM_START* Dos9_AllocParsedStreamStart(void);
PARSED_STREAM*       Dos9_AllocParsedStream(PARSED_STREAM* lppsStream);

void                 Dos9_FreeParsedStream(PARSED_STREAM* lppsStream);
void                 Dos9_FreeParsedStreamStart(PARSED_STREAM_START* lppssStart);

#endif // DOS9_PARSE_H
