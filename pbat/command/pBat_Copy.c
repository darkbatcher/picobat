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

// #define PBAT_DBG_MODE
#include "../core/pBat_Debug.h"

#include "../errors/pBat_Errors.h"

#include "pBat_Copy.h"

/* COPY [/R] [/-Y | /Y] [/A[:]attributes] source [+] [...] destination
   MOVE [/R] [/-Y | /Y] [/A[:]attributes] source  [...] destination

    - [/-Y | /Y] : Prompt the user.

    - /A[:]Attributes : Select the file to be copied in an attribute basis.

    - /R : Copy the whole tree starting in the static part of the program. If
    multiple sources are specified, then the trees will be merged. If sources
    is a regular expression, relative names in the destination folder begin
    at the end of the static part of the regular expression. For example, if
    COPY encounters :

        path/to/folder/*.*
        \_____________/\_/
          Static part   Regexp

    Then 'path/to/folder/' will be stripped from the path of object encountered
    in 'path/to/folder' to make the relative path on destination folder.

    - source ...: A list of files that will be copied to destination. Source can
    obviously include regular expressions. If the command invoked is COPY and
    if any '+' is specified between any of the filenames, the command will
    consider that destination is the path of a file in which every file
    matching any of the source expressions will be concatenated, starting from
    the first source expression. Cmd used to accept filename concatenated with
    a '+' such as file1+file2+file3+...+fileN. This is broken syntax since there
    is *absolutely* no reason why a file could not contain any '+' sign in its
    name. Thus pBat only supports detached names.

    - destination : The location where the files will be copied/moved. If
    source is a single file, then destination is considered to be the destination
    filename unless (a) destination refers to a folder (b) destination is
    terminated by either '/' or '\\' or (c) multiples files must be moved to
    destination. If multiples files must be copied to a destination but neither of
    (a) or (b) are met, then the file are concatenated inside a file named `destination`
    No regular expression accepted.

*/

#ifdef WIN32
#define IS_DIR_DELIM(c) (c=='/' || c=='\\')
#else
#define IS_DIR_DELIM(c) (c=='/')
#endif // WIN32

int pBat__EndWithDirectoryMark(const char* dir)
{
    char* c="";

    while (*dir)
        c = dir ++;

    return PBAT_TEST_SEPARATOR(c);
}

int pBat_CmdCopy(char* line)
{
	FILELIST *files=NULL,
             *next=NULL,
             *end;

    ESTR *param=pBat_EsInit_Cached();

    int i=0,
        len=0,
        flags=(*line == 'c' || *line == 'C') ? 0 : PBAT_COPY_MOVE,
        status=0;

    char **file,
         **tmp;

    size_t filesize = 64;

    char *str;
    short attr=0;

    line +=4;

    if ((file = malloc(filesize * sizeof(char*))) == NULL) {

        pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                __FILE__ "/pBat_CmdCopy()", 0);
        status = PBAT_FAILED_ALLOCATION;
        goto end;

    }

    while ((line = pBat_GetNextParameterEs(line, param))) {

        str = pBat_EsToChar(param);

        if (!stricmp("/Y", str)
            || !stricmp("/-Y", str)) {

            flags |= (*(str+1)=='-') ? PBAT_COPY_SILENCE : 0;

        } else if (!strnicmp("/A", str, 2)) {

            str += 2;

            if (*str == ':')
                str ++;

            attr = pBat_MakeFileAttributes(str);

        } else if (!stricmp("/R", str)) {

            flags |= PBAT_COPY_RECURSIVE;

        } else if (!strcmp(str, "/?")) {

            if (flags & PBAT_COPY_MOVE)
                pBat_ShowInternalHelp(PBAT_HELP_MOVE);
            else
                pBat_ShowInternalHelp(PBAT_HELP_COPY);

            goto end;

        } else {

            if (!strcmp(str, "+")) {

                if (flags & PBAT_COPY_MOVE) {

                    pBat_ShowErrorMessage(PBAT_UNEXPECTED_ELEMENT, str, FALSE);
                    status = PBAT_UNEXPECTED_ELEMENT;
                    goto end;

                }

                flags |= PBAT_COPY_CAT;
                continue;
            }

            if (len >= filesize) {

                filesize *= 2;

                if ((tmp = realloc(file, filesize * sizeof(char*))) == NULL) {

                    pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                            __FILE__ "/pBat_CmdCopy()", 0);
                    status = PBAT_FAILED_ALLOCATION;
                    goto end;

                }

                file = tmp;

            }

            if ((file[len] = pBat_FullPathDup(str)) == NULL) {

                pBat_ShowErrorMessage(PBAT_FAILED_ALLOCATION,
                                        __FILE__ "/pBat_CmdCopy()", 0);
                status = PBAT_FAILED_ALLOCATION;
                goto end;

            }

            len ++;

        }
    }

    if (len < 2) {

        pBat_ShowErrorMessage(PBAT_EXPECTED_MORE, "COPY", FALSE);
        status = PBAT_EXPECTED_MORE;

        goto end;
    }

    if ((flags & PBAT_COPY_RECURSIVE)
        && (flags & PBAT_COPY_CAT)) {

        pBat_ShowErrorMessage(PBAT_INCOMPATIBLE_ARGS, "/R, + (COPY)", FALSE);

        status = PBAT_INCOMPATIBLE_ARGS;
        goto end;

    }

    if (flags & PBAT_COPY_RECURSIVE) {

        for (i=0;i < len - 1 ; i++)
            status = pBat_CmdCopyRecursive(file[i],
                        file[len-1], attr, &flags);

        goto end;

    }

    for (i=0;i < len - 1; i++) {

        next = pBat_GetMatchFileList(file[i], PBAT_SEARCH_DEFAULT);

        if (next == NULL) {

            pBat_ShowErrorMessage(PBAT_NO_MATCH,
                                    file[i], FALSE);
            status = PBAT_NO_MATCH;

            goto end;

        }

        if (files == NULL) {
            files = next;
        } else {
            end->lpflNext = next;
        }

        while (next->lpflNext)
            next = next->lpflNext;

        end = next;

    }

    end = files;

    while (end) {
        end = end->lpflNext;
    }

    /* Get matching attributes */
    pBat_AttributesSplitFileList(attr | PBAT_ATTR_NO_DIR,
                                    files,
                                    &files,
                                    &next);

    end = files;

    while (end) {
        end = end->lpflNext;
    }

    end = files;

    /* actual matching items */
    while (end && (end = end->lpflNext) && (i++ < 2));

    /* Cmd does not accept several file names used in with a
       single *COPY* but for it, several files from a regular expression
       mean concatenation of file, which is somehow strange. */
    if (pBat_DirExists(file[len-1])
        || pBat__EndWithDirectoryMark(file[len-1])) {
        flags |= PBAT_COPY_MULTIPLE;
    }

    if (i == 3) {

        if (!(flags & PBAT_COPY_MULTIPLE)
            && !(flags & PBAT_COPY_MOVE))
            flags |= PBAT_COPY_CAT;
        else
            flags |= PBAT_COPY_MULTIPLE;
    }


    while (files) {

        status |= pBat_CmdCopyFile(files->lpFileName, file[len-1], &flags);

        files = files->lpflNext;
    }

end:
    pBat_FreeFileList(files);
    pBat_FreeFileList(next);
    pBat_EsFree_Cached(param);

    if (file) {
        for (i=0;i < len; i++)
            free(file[i]);

        free(file);
    }

    return status;
}

int pBat_CmdCopyFile(char* file, const char* dest, int* flags)
{
    int ok=1;
    int status = PBAT_NO_ERROR;

    char  name[FILENAME_MAX],
          ext[FILENAME_MAX];

    ESTR* dest_real = pBat_EsInit_Cached();

    if (((*flags & PBAT_COPY_MULTIPLE)
            && !(*flags & PBAT_COPY_CAT))) {

        pBat_SplitPath(file, NULL, NULL, name, ext);

        strncat(name, ext, FILENAME_MAX - strlen(name));
        name[FILENAME_MAX-1] = '\0';

        pBat_MakePath(dest_real, 2, dest, name);

    } else {

        pBat_EsCpy(dest_real, dest);

    }

    if (pBat_FileExists(pBat_EsToChar(dest_real))
        && !(*flags & PBAT_COPY_SILENCE)) {

        ok = pBat_AskConfirmation(PBAT_ASK_YNA | PBAT_ASK_DEFAULT_N
                                    | PBAT_ASK_INVALID_REASK, NULL,
                                    lpCopyConfirm,
                                    pBat_EsToChar(dest_real),
                                    file
                                    );

        switch (ok) {

            case PBAT_ASK_ALL:
                *flags |= PBAT_COPY_SILENCE;
            case PBAT_ASK_YES:
                ok=1;
                break;

            case PBAT_ASK_NO:
                ok=0;

        }
    }

    if (ok == 1) {

        if (*flags & PBAT_COPY_MOVE) {

            status = pBat_MoveFile(file, dest_real->str);

        } else if (*flags & PBAT_COPY_CAT) {

            status = pBat_CatFile(file, dest_real->str, flags);

        } else {

            status = pBat_CopyFile(file, dest_real->str);

        }

    }

    pBat_EsFree_Cached(dest_real);

    return status;
}

int pBat_CmdCopyRecursive(char* file, const char* dest, short attr, int* flags)
{
    FILELIST *files=NULL,
             *dirs=NULL,
             *end=NULL,
             *item;

    ESTR* real_dest = pBat_EsInit_Cached();

    int  status = 0;
    size_t size  = pBat_GetStaticLength(file);

    if (size != 0)
        size +=1;

    if (!(files = pBat_GetMatchFileList(file, PBAT_SEARCH_DIR_MODE
                                             | PBAT_SEARCH_RECURSIVE
                                             | PBAT_SEARCH_NO_PSEUDO_DIR))) {

        pBat_ShowErrorMessage(PBAT_NO_MATCH,
                              file,
                              FALSE);

        status = -1;
        goto end;

    }

    pBat_AttributesSplitFileList(attr,
                                 files,
                                 &files,
                                 &end
                                 );

    pBat_AttributesSplitFileList(PBAT_ATTR_NO_DIR,
                                 files,
                                 &files,
                                 &dirs
                                 );

    item = dirs;

    /* duplicate directories */
    while (item) {

        pBat_MakePath(real_dest, 2, dest, (item->lpFileName)+(size));
        status |= pBat_CmdMakeDirs(real_dest->str);

        item = item->lpflNext;

    }

    item = files;

    /* copy file */
    while (item) {

        pBat_MakePath(real_dest, 2, dest, (item->lpFileName)+(size));
        status |= pBat_CmdCopyFile(item->lpFileName, real_dest->str, flags);

        item = item->lpflNext;

    }

    if (*flags & PBAT_COPY_MOVE) {

        item = dirs;

        while (item) {

            pBat_MakePath(real_dest, 2, dest, (item->lpFileName)+(size+1));

            status = pBat_Rmdir(real_dest->str);

            item = item->lpflNext;

        }

    }

end:
    pBat_EsFree_Cached(real_dest);
    pBat_FreeFileList(files);
    pBat_FreeFileList(end);
    pBat_FreeFileList(dirs);

    return status;

}

int pBat_MoveFile(const char* file, const char* dest)
{
    if ((pBat_FileExists(dest)
         && remove(dest))
        || rename(file, dest)) {

        pBat_ShowErrorMessage(PBAT_UNABLE_MOVE | PBAT_PRINT_C_ERROR,
                                file,
                                0);

        return PBAT_UNABLE_MOVE;
    }

    return PBAT_NO_ERROR;
}

int pBat_CopyFile(const char* file, const char* dest)
{
    int old, new;
    char buf[2048];
    size_t count, writen;
    struct stat info;

#ifdef WIN32
    old = open(file, O_RDONLY | O_BINARY ,0);
#else
    old = open(file, O_RDONLY,0);
#endif

    fstat(old, &info);

    if (old == -1) {

        pBat_ShowErrorMessage(PBAT_UNABLE_COPY
                                | PBAT_PRINT_C_ERROR, file, 0);


        return -1;

    }

#ifdef WIN32
    new = open(dest, O_WRONLY | O_CREAT  | O_TRUNC | O_BINARY, info.st_mode);
#else
    new = open(dest, O_WRONLY | O_CREAT | O_TRUNC, info.st_mode);
#endif

    if (new == -1) {

        close(old);

        pBat_ShowErrorMessage(PBAT_UNABLE_COPY
                                | PBAT_PRINT_C_ERROR, file, 0);


        return PBAT_UNABLE_COPY;

    }

    while ((count = read(old, buf, sizeof(buf))) != 0) {

        if (count == -1) {


            close(old);
            close(new);

            pBat_ShowErrorMessage(PBAT_UNABLE_COPY
                                    | PBAT_PRINT_C_ERROR, file, 0);

            return PBAT_UNABLE_COPY;

        }

        writen = 0;

        while ((writen += write(new, buf + writen, count - writen)) != count) {

            if (writen == -1) {

                close(old);
                close(new);

                pBat_ShowErrorMessage(PBAT_UNABLE_COPY
                                        | PBAT_PRINT_C_ERROR, file, 0);

                return PBAT_UNABLE_COPY;

            }

        }

    }

    #ifdef WIN32
        _commit(new);
    #else
        fsync(new);
    #endif // WIN32

    close(old);
    close(new);

    return PBAT_NO_ERROR;

}

int pBat_CatFile(const char* file, const char* dest, int* flags)
{
    int old, new, attr;
    char buf[2048];
    size_t count, writen;
    struct stat info;

#ifdef WIN32
    old = open(file, O_RDONLY | O_BINARY ,0);
#else
    old = open(file, O_RDONLY,0);
#endif

    fstat(old, &info);

    if (old == -1) {

        pBat_ShowErrorMessage(PBAT_UNABLE_CAT
                                | PBAT_PRINT_C_ERROR, file, 0);


        return PBAT_UNABLE_CAT;

    }

#ifdef WIN32
    attr = O_WRONLY | O_BINARY | O_CREAT ;
#else
    attr = O_WRONLY | O_CREAT ;
#endif

    if (*flags & PBAT_COPY_CAT_2ND) {

        attr |= O_APPEND;

    } else {

        *flags |= PBAT_COPY_CAT_2ND;
        attr |= O_TRUNC;

    }

    new = open(dest, attr, info.st_mode);

    if (new == -1) {

        close(old);

        pBat_ShowErrorMessage(PBAT_UNABLE_CAT
                                | PBAT_PRINT_C_ERROR, file, 0);


        return PBAT_UNABLE_CAT;

    }

    while ((count = read(old, buf, sizeof(buf))) != 0) {

        if (count == -1) {


            close(old);
            close(new);

            pBat_ShowErrorMessage(PBAT_UNABLE_CAT
                                    | PBAT_PRINT_C_ERROR, file, 0);

            return PBAT_UNABLE_CAT;

        }

        writen = 0;

        while ((writen += write(new, buf + writen, count - writen)) != count) {

            if (writen == -1) {

                close(old);
                close(new);

                pBat_ShowErrorMessage(PBAT_UNABLE_CAT
                                        | PBAT_PRINT_C_ERROR, file, 0);

                return PBAT_UNABLE_CAT;

            }

        }

    }

    #ifdef WIN32
        _commit(new);
    #else
        fsync(new);
    #endif // WIN32

    close(old);
    close(new);

    return PBAT_NO_ERROR;

}
