/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
 *
 */

#ifndef _XOPEN_SOURCE
#define _XOPEN_SOURCE 700
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>

#if defined(WIN32)
#include <io.h>
#endif

#include <libpBat.h>

#include "../core/pBat_Core.h"
#include "../../config.h"

#include "pBat_Wc.h"

#include "../lang/pBat_Lang.h"
#include "../lang/pBat_ShowHelp.h"

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

/* WC /L /C /M /W [files ...]

   Count the number of lines, characters, bytes or words from
   a file or a stream. If none of FILES are specified, then
   the computation occurs on stdin.

        /L : count lines
        /C : count bytes
        /M : count characters
        /W : count words

 */

int pBat_FileCounts(int mode, const char* file, struct wc_count_t* cnt)
{
    int fd,
        inword = 0; /* true if within a word */
    char buf[1024], *pch;
    size_t size;

#ifndef WIN32
#define O_BINARY 0
#endif // WIN32

    if (file == NULL) {

        fd = fileno(fInput);

    } else if ((fd = open(file, O_RDONLY | O_BINARY)) == -1) {

        pBat_ShowErrorMessage(PBAT_FILE_ERROR, file,0);
        return PBAT_FILE_ERROR;

    }

    /* reset the count fields */
    cnt->bytes = 0;
    cnt->chars = 0;
    cnt->words = 0;
    cnt->lines = 0;

    while ((size = read(fd, buf, sizeof(buf))) && size != -1) {

        /* add bytes */
        cnt->bytes += size;

        /* read a bunch of bytes and count items */
        pch = buf;

        /* process the buffer */
        while (pch < buf + size) {

            /* count utf-8 if utf8 is enabled or just bytes if it is not
               the case;

               We do not bother of actual conformance of the file. */
#if !defined(WIN32) || defined(PBAT_USE_LIBCU8)
            if (!(*pch & 0x80) || (*pch & 0xC0) == 0xC0)
#endif
                cnt->chars ++;

            switch (*pch) {
            case '\n':
                /* Counting newlines only relies on the numbers of line feeds
                   encountered by the program. We do not take account of
                   chariot return since this will bloat the system.

                   Anyway, one may wonder what does "counting line" should be
                   accounted for in a file that does not respect the platform
                   end of line standards.

                   */
                cnt->lines ++;

            case ' ':
            case '\t':
            case '\r':
            case ',':
            case '.':
            case ';':
                if (inword) {
                    inword = 0;
                    cnt->words ++;
                }
                break;

            default:
                inword = 1;
            }

            pch ++;

        }

    }

    /* Wait ... Fool, were you just about to close a file that could refer
       to stdin ? double check it's not so before closing the file */
    if (fd != fileno(fInput)) {
        close(fd);
    } else if (isatty(fileno(fInput))) {
        clearerr(fInput);
    }

    return 0;
}

void pBat_AddCounts(struct wc_count_t* res, const struct wc_count_t* val)
{
    res->lines += val->lines;
    res->words += val->words;
    res->bytes += val->bytes;
    res->chars += val->chars;
}

void pBat_PrintCounts(int mode, struct wc_count_t* cnt, const char* file)
{
    /* Do not pretty print, gnu does so, but we won't */

    if (mode & PBAT_WC_LINES)
        fprintf(fOutput, "%lu ", cnt->lines);

    if (mode & PBAT_WC_WORDS)
        fprintf(fOutput, "%lu ", cnt->words);

    if (mode & PBAT_WC_BYTES)
        fprintf(fOutput, "%lu ", cnt->bytes);

    if (mode & PBAT_WC_CHARS)
        fprintf(fOutput, "%lu ", cnt->chars);

    if (file)
        fprintf(fOutput, "%s", file);

    fputs(PBAT_NL, fOutput);
}

int pBat_CmdWc(char* line)
{
    ESTR* param = pBat_EsInit_Cached();
    FILELIST *match = NULL,
             *end,
             *dirs,
             *item;
    int mode = 0,
        nb = 0,
        size,
        status = PBAT_NO_ERROR,
        r = 0;

    struct wc_count_t total = {0, 0, 0, 0}, cnt;

    line += 2;

    while ((line = pBat_GetNextParameterEs(line, param))) {

        if (*(param->str) == '/' && *(param->str + 1)
            && *(param->str + 2) == '\0') {

            switch (*(param->str + 1)) {

            case '?':
                pBat_ShowInternalHelp(PBAT_HELP_WC);
                goto end;

            case 'L':
            case 'l':
                mode |= PBAT_WC_LINES;
                break;

            case 'C':
            case 'c':
                mode |= PBAT_WC_BYTES;
                break;

            case 'M':
            case 'm':
                mode |= PBAT_WC_CHARS;
                break;

            case 'W':
            case 'w':
                mode |= PBAT_WC_WORDS;
                break;

            default:
                pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, param->str, 0);
                status = PBAT_UNEXPECTED_ELEMENT;

                goto error;

            }

        } else {

            if (!PBAT_TEST_ABSOLUTE_PATH(param->str))
                size = strlen(lpCurrentDir) + 1;
            else
                size = 0;


            /* Search for the appropriate files */
            if (!(item = pBat_GetMatchFileList(pBat_EsToFullPath(param),
                                               PBAT_SEARCH_NO_PSEUDO_DIR))) {

                pBat_ShowErrorMessage(PBAT_NO_MATCH, param->str, 0);
                status = PBAT_NO_MATCH;

                goto error;

            }

            /* Select files that are not dirs */
            pBat_AttributesSplitFileList(PBAT_ATTR_NO_DIR, item, &item, &dirs);

            if (dirs)
                pBat_FreeFileList(dirs);

            /* return error if no file match */
            if (item == NULL) {

                pBat_ShowErrorMessage(PBAT_NO_MATCH, param->str, 0);
                status = PBAT_NO_MATCH;

                goto error;

            }

            if (match) {
                end->lpflNext = item;
            } else {
                match = item;
            }

            /* Find the last item of the list */
            while (item) {
                end = item;
                item->stFileStats.st_uid = size; /* hack : use an unused file info to
                                                   store length of the added prefix  */
                item = item->lpflNext;
            }
        }
    }

    /* Display no error about file given ... */
    if (match == NULL) {
        /* Something to do with stdin */
        status = pBat_FileCounts(mode, NULL, &cnt);
        pBat_PrintCounts(mode, &cnt, NULL);
    }

    /* Count bytes by default */
    if (mode == 0)
        mode = PBAT_WC_BYTES;

    /* Perform count on files */
    item = match;
    while (item) {

        if (mode == PBAT_WC_BYTES) {
            /* If the user only requests file size, no need to actually open
               the file as it is one of the few thing given both by windows
               an *nix.

               Thus, do not request any kind of read */

            total.bytes += pBat_GetFileSize(item);

            fprintf(fOutput, "%lu %s" PBAT_NL, pBat_GetFileSize(item),
                        item->lpFileName + (size_t)item->stFileStats.st_uid);

        } else if (!(r = pBat_FileCounts(mode, item->lpFileName, &cnt))) {
            /* Reading the whole file is required */

            pBat_PrintCounts(mode, &cnt,
                                item->lpFileName + (size_t)item->stFileStats.st_uid);
            pBat_AddCounts(&total, &cnt);

        }

        status |= r;

        /* remember we number of files we parsed successfully */
        nb ++;

        item = item->lpflNext;
    }

    if (nb > 1) {
        /* print the total */
        pBat_PrintCounts(mode, &total, "Total");

    }

error:
end:
    if (match)
        pBat_FreeFileList(match);

    pBat_EsFree_Cached(param);
    return status;

}
