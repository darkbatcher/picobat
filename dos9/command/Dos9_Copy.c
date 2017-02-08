/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#include <matheval.h>
#include <inteval.h>

#include <libDos9.h>

#include "../core/Dos9_Core.h"

#include "Dos9_Commands.h"

#include "../lang/Dos9_Lang.h"
#include "../lang/Dos9_ShowHelp.h"

// #define DOS9_DBG_MODE
#include "../core/Dos9_Debug.h"

#include "../errors/Dos9_Errors.h"

#include "Dos9_Copy.h"

/* COPY [/R] [/-Y | /Y] [/A[:]attributes] source [...] destination
   MOVE [/R] [/-Y | /Y] [/A[:]attributes] source [...] destination

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
    obviously include regular expressions.

    - destination : The location where the files will be copied. If
    source is a single file, then destination is considered to be the destination
    filename unless (a) destination refers to a folder (b) destination is
    terminated by either '/' or '\\' or (c) multiples files must be copied to
    destination. No regular expression accepted.

    Note : COPY command only copy files, so that it will not create directories in
    destination files. To duplicate a tree, rather use the XCOPY command.

*/

int Dos9_CmdCopy(char* line)
{
	FILELIST *files=NULL,
             *next=NULL,
             *end;

    ESTR *param=Dos9_EsInit(),
         *file[FILENAME_MAX];

    int i=0,
        len=0,
        flags=((*line | 'c'-'C' )== 'c' ) ? 0 : DOS9_COPY_MOVE,
        status=0;

    char *str;
    short attr=0;

    line +=4;

    while (line = Dos9_GetNextParameterEs(line, param)) {

        str = Dos9_EsToChar(param);

        if (!stricmp("/Y", str)
            || !stricmp("/-Y", str)) {

            flags |= (*(str+1)=='-') ? DOS9_COPY_SILENCE : 0;

        } if (!strnicmp("/A", str, 2)) {

            str += 2;

            if (*str == ':')
                str ++;

            attr = Dos9_MakeFileAttributes(str);

        } else if (!stricmp("/R", str)) {

            flags |= DOS9_COPY_RECURSIVE;

        } else if (!strcmp(str, "/?")) {

            Dos9_ShowInternalHelp(DOS9_HELP_COPY);
            goto end;

        } else {

            if (len >= FILENAME_MAX) {

                Dos9_ShowErrorMessage(DOS9_TOO_MANY_ARGS,
                                      (flags & DOS9_COPY_MOVE) ? "MOVE": "COPY",
                                      FALSE);

                status = -1;
                goto end;

            }

            file[len] = Dos9_EsInit();
            Dos9_EsCpy(file[len], str);

            len ++;

        }
    }

    if (len < 2) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "COPY", FALSE);
        status -1;

        goto end;
    }

    if (flags & DOS9_COPY_RECURSIVE) {

        for (i=0;i < len - 1 ; i++)
            status = Dos9_CmdCopyRecursive(Dos9_EsToChar(file[i]),
                        Dos9_EsToChar(file[len-1]), attr, &flags);

        goto end;

    }

    for (i=0;i < len - 1; i++) {

        next = Dos9_GetMatchFileList(Dos9_EsToChar(file[i]), DOS9_SEARCH_DEFAULT);

        if (next == NULL) {

            Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                                    Dos9_EsToChar(file[i]), FALSE);
            status = -1;

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
    Dos9_AttributesSplitFileList(attr | DOS9_ATTR_NO_DIR,
                                    files,
                                    &files,
                                    &next);

    end = files;

    while (end) {
        end = end->lpflNext;
    }

    end = files;

    while (end && (end = end->lpflNext) && (i++ < 2));

    if (i == 3)
        flags |= DOS9_COPY_MULTIPLE;

    while (files) {

        status |= Dos9_CmdCopyFile(files->lpFileName, Dos9_EsToChar(file[len-1]), &flags);

        files = files->lpflNext;
    }

end:
    Dos9_FreeFileList(files);
    Dos9_FreeFileList(next);
    Dos9_EsFree(param);

    for (i=0;i < len; i++)
        Dos9_EsFree(file[i]);

    return status;
}

#ifdef WIN32
#define IS_DIR_DELIM(c) (c=='/' || c=='\\')
#else
#define IS_DIR_DELIM(c) (c=='/')
#endif // WIN32

int Dos9__EndWithDirectoryMark(const char* dir)
{
    int c;

    while (*dir)
        c = *(dir ++);

    return IS_DIR_DELIM(c);
}

int Dos9_CmdCopyFile(const char* file, const char* dest, int* flags)
{
    int ok=1;

    char  name[FILENAME_MAX],
          ext[FILENAME_MAX];

    ESTR* dest_real = Dos9_EsInit();

    if ((*flags & DOS9_COPY_MULTIPLE)
        || Dos9_DirExists(dest)
        || Dos9__EndWithDirectoryMark(dest)) {

        Dos9_SplitPath(file, NULL, NULL, name, ext);

        strncat(name, ext, FILENAME_MAX);
        name[FILENAME_MAX-1] = '\0';

        Dos9_MakePath(dest_real, 2, dest, name);

    } else {

        Dos9_EsCpy(dest_real, dest);

    }

    if (Dos9_FileExists(Dos9_EsToChar(dest_real))
        && !(*flags & DOS9_COPY_SILENCE)) {

        ok = Dos9_AskConfirmation(DOS9_ASK_YNA | DOS9_ASK_DEFAULT_N
                                    | DOS9_ASK_INVALID_REASK,
                                    lpCopyConfirm,
                                    Dos9_EsToChar(dest_real),
                                    file
                                    );

        switch (ok) {

            case DOS9_ASK_ALL:
                *flags |= DOS9_COPY_SILENCE;
            case DOS9_ASK_YES:
                ok=1;
                break;

            case DOS9_ASK_NO:
                ok=0;

        }
    }

    if (ok == 1) {

        if (*flags & DOS9_COPY_MOVE) {

            Dos9_MoveFile(file, dest_real->str);

        } else {

            Dos9_CopyFile(file, dest_real->str);
        }

    }

    Dos9_EsFree(dest_real);

    return 0;
}

int Dos9_CmdCopyRecursive(const char* file, const char* dest, short attr, int* flags)
{
    FILELIST *files=NULL,
             *dirs=NULL,
             *end=NULL,
             *item;

    ESTR* real_dest = Dos9_EsInit();

    int  status = 0;
    size_t size;

    if (size = Dos9_GetStaticLength(file))
        size ++;

    if (!(files = Dos9_GetMatchFileList(file, DOS9_SEARCH_DIR_MODE
                                             | DOS9_SEARCH_RECURSIVE
                                             | DOS9_SEARCH_NO_PSEUDO_DIR))) {

        Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                              file,
                              FALSE);

        status = -1;
        goto end;

    }

    Dos9_AttributesSplitFileList((attr & ~ DOS9_ATTR_NO_DIR),
                                 files,
                                 &files,
                                 &end
                                 );

    Dos9_AttributesSplitFileList(attr,
                                 files,
                                 &files,
                                 &dirs
                                 );

    item = dirs;

    while (item) {

        Dos9_MakePath(real_dest, 2, dest, (item->lpFileName)+(size+1));


        status |= Dos9_CmdMakeDirs(real_dest->str);

        item = item->lpflNext;

    }

    item = files;

    while (item) {

        Dos9_MakePath(real_dest, 2, dest, (item->lpFileName)+(size));

        status |= Dos9_CmdCopyFile(item->lpFileName, real_dest->str, flags);

    }

    if (*flags & DOS9_COPY_MOVE) {

        item = dirs;

        while (item) {

            Dos9_MakePath(real_dest, 2, dest, (item->lpFileName)+(size+1));

            status = Dos9_Rmdir(real_dest->str);

            item = item->lpflNext;

        }

    }

end:
    Dos9_EsFree(real_dest);
    Dos9_FreeFileList(files);
    Dos9_FreeFileList(end);
    Dos9_FreeFileList(dirs);

    return status;

}

int Dos9_MoveFile(const char* file, const char* dest)
{

    if (rename(file, dest)) {

        Dos9_ShowErrorMessage(DOS9_UNABLE_MOVE | DOS9_PRINT_C_ERROR,
                                file,
                                0);

        return -1;
    }

    return 0;
}

int Dos9_CopyFile(const char* file, const char* dest)
{
    int old, new;
    char buf[2048];
    size_t count, writen;
    struct stat info;

    printf("Copy %s to %s\n", file, dest);

    old = open(file, O_RDONLY,0);

    fstat(old, &info);

    if (old == -1) {

        Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                | DOS9_PRINT_C_ERROR, file, 0);


        return -1;

    }


    new = open(dest, O_WRONLY | O_CREAT, info.st_mode);

    if (new == -1) {

        close(old);

        Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                | DOS9_PRINT_C_ERROR, file, 0);


        return -1;

    }

    while ((count = read(old, buf, sizeof(buf))) != 0) {

        printf("Read = %d", count);

        if (count == -1) {


            close(old);
            close(new);

            Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                    | DOS9_PRINT_C_ERROR, file, 0);

            return -1;

        }

        writen = 0;

        while ((writen += write(new, buf + writen, count - writen)) != count) {

            if (writen == -1) {

                close(old);
                close(new);

                Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                        | DOS9_PRINT_C_ERROR, file, 0);

                return -1;

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

    return 0;

}
