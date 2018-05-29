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

/* COPY [/R] [/-Y | /Y] [/A[:]attributes] source [+] [...] destination
   MOVE [/R] [/-Y | /Y] [/A[:]attributes] source  [...] destination

    - [/-Y | /Y] : Prompt the user.

    - /A[:]Attributes : Select the file to be copied in an attribute basis.

    - /R : Copy the whole tree starting in the static part of the program. If
    multiple sources are specified, then the trees will be merged. If sources
    is a regular expression, relative names in the destination folder begin
    at the end of the static part of the regular expression. For example, if
    COPY encounters :

        path/to/folder/ *.*
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
    name. Thus Dos9 only supports detached names.

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

int Dos9__EndWithDirectoryMark(const char* dir)
{
    int c;

    while (*dir)
        c = *(dir ++);

    return IS_DIR_DELIM(c);
}

int Dos9_CmdCopy(char* line)
{
	FILELIST *files=NULL,
             *next=NULL,
             *end;

    ESTR *param=Dos9_EsInit();

    int i=0,
        len=0,
        flags=(*line == 'c' || *line == 'C') ? 0 : DOS9_COPY_MOVE,
        status=0;

    char **file,
         **tmp;

    size_t filesize = 64;

    char *str;
    short attr=0;

    line +=4;

    if ((file = malloc(filesize * sizeof(char*))) == NULL) {

        Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                __FILE__ "/Dos9_CmdCopy()", 0);
        status = DOS9_FAILED_ALLOCATION;
        goto end;

    }

    while ((line = Dos9_GetNextParameterEs(line, param))) {

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

            if (flags & DOS9_COPY_MOVE)
                Dos9_ShowInternalHelp(DOS9_HELP_MOVE);
            else
                Dos9_ShowInternalHelp(DOS9_HELP_COPY);

            goto end;

        } else {

            if (!strcmp(str, "+")) {

                if (flags & DOS9_COPY_MOVE) {

                    Dos9_ShowErrorMessage(DOS9_UNEXPECTED_ELEMENT, str, FALSE);
                    status = DOS9_UNEXPECTED_ELEMENT;
                    goto end;

                }

                flags |= DOS9_COPY_CAT;
                continue;
            }

            if (len >= filesize) {

                filesize *= 2;

                if ((tmp = realloc(file, filesize * sizeof(char*))) == NULL) {

                    Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                            __FILE__ "/Dos9_CmdCopy()", 0);
                    status = DOS9_FAILED_ALLOCATION;
                    goto end;

                }

                file = tmp;

            }

            if ((file[len] = Dos9_FullPathDup(str)) == NULL) {

                Dos9_ShowErrorMessage(DOS9_FAILED_ALLOCATION,
                                        __FILE__ "/Dos9_CmdCopy()", 0);
                status = DOS9_FAILED_ALLOCATION;
                goto end;

            }

            len ++;

        }
    }

    if (len < 2) {

        Dos9_ShowErrorMessage(DOS9_EXPECTED_MORE, "COPY", FALSE);
        status = DOS9_EXPECTED_MORE;

        goto end;
    }

    if ((flags & DOS9_COPY_RECURSIVE)
        && (flags & DOS9_COPY_CAT)) {

        Dos9_ShowErrorMessage(DOS9_INCOMPATIBLE_ARGS, "/R, + (COPY)", FALSE);

        status = DOS9_INCOMPATIBLE_ARGS;
        goto end;

    }

    if (flags & DOS9_COPY_RECURSIVE) {

        for (i=0;i < len - 1 ; i++)
            status = Dos9_CmdCopyRecursive(file[i],
                        file[len-1], attr, &flags);

        goto end;

    }

    for (i=0;i < len - 1; i++) {

        next = Dos9_GetMatchFileList(file[i], DOS9_SEARCH_DEFAULT);

        if (next == NULL) {

            Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                                    file[i], FALSE);
            status = DOS9_NO_MATCH;

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

    /* actual matching items */
    while (end && (end = end->lpflNext) && (i++ < 2));

    /* Cmd does not accept several file names used in with a
       single *COPY* but for it, several files from a regular expression
       mean concatenation of file, which is somehow strange. */
    if (Dos9_DirExists(file[len-1])
        || Dos9__EndWithDirectoryMark(file[len-1])) {
        flags |= DOS9_COPY_MULTIPLE;
    }

    if (i == 3) {

        if (!(flags & DOS9_COPY_MULTIPLE)
            && !(flags & DOS9_COPY_MOVE))
            flags |= DOS9_COPY_CAT;
        else
            flags |= DOS9_COPY_MULTIPLE;
    }


    while (files) {

        status |= Dos9_CmdCopyFile(files->lpFileName, file[len-1], &flags);

        files = files->lpflNext;
    }

end:
    Dos9_FreeFileList(files);
    Dos9_FreeFileList(next);
    Dos9_EsFree(param);

    if (file) {
        for (i=0;i < len; i++)
            free(file[i]);

        free(file);
    }

    return status;
}

int Dos9_CmdCopyFile(char* file, const char* dest, int* flags)
{
    int ok=1;
    int status = DOS9_NO_ERROR;

    char  name[FILENAME_MAX],
          ext[FILENAME_MAX];

    ESTR* dest_real = Dos9_EsInit();

    if (((*flags & DOS9_COPY_MULTIPLE)
            && !(*flags & DOS9_COPY_CAT))) {

        Dos9_SplitPath(file, NULL, NULL, name, ext);

        strncat(name, ext, FILENAME_MAX - strlen(name));
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

            status = Dos9_MoveFile(file, dest_real->str);

        } else if (*flags & DOS9_COPY_CAT) {

            status = Dos9_CatFile(file, dest_real->str, flags);

        } else {

            status = Dos9_CopyFile(file, dest_real->str);

        }

    }

    Dos9_EsFree(dest_real);

    return status;
}

int Dos9_CmdCopyRecursive(char* file, const char* dest, short attr, int* flags)
{
    FILELIST *files=NULL,
             *dirs=NULL,
             *end=NULL,
             *item;

    ESTR* real_dest = Dos9_EsInit();

    int  status = 0;
    size_t size  = Dos9_GetStaticLength(file);

    if (size != 0)
        size +=1;

    if (!(files = Dos9_GetMatchFileList(file, DOS9_SEARCH_DIR_MODE
                                             | DOS9_SEARCH_RECURSIVE
                                             | DOS9_SEARCH_NO_PSEUDO_DIR))) {

        Dos9_ShowErrorMessage(DOS9_NO_MATCH,
                              file,
                              FALSE);

        status = -1;
        goto end;

    }

    Dos9_AttributesSplitFileList(attr,
                                 files,
                                 &files,
                                 &end
                                 );

    Dos9_AttributesSplitFileList(DOS9_ATTR_NO_DIR,
                                 files,
                                 &files,
                                 &dirs
                                 );

    item = dirs;

    /* duplicate directories */
    while (item) {

        Dos9_MakePath(real_dest, 2, dest, (item->lpFileName)+(size));
        status |= Dos9_CmdMakeDirs(real_dest->str);

        item = item->lpflNext;

    }

    item = files;

    /* copy file */
    while (item) {

        Dos9_MakePath(real_dest, 2, dest, (item->lpFileName)+(size));
        status |= Dos9_CmdCopyFile(item->lpFileName, real_dest->str, flags);

        item = item->lpflNext;

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

        return DOS9_UNABLE_MOVE;
    }

    return DOS9_NO_ERROR;
}

int Dos9_CopyFile(const char* file, const char* dest)
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

        Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                | DOS9_PRINT_C_ERROR, file, 0);


        return -1;

    }

#ifdef WIN32
    new = open(dest, O_WRONLY | O_CREAT  | O_TRUNC | O_BINARY, info.st_mode);
#else
    new = open(dest, O_WRONLY | O_CREAT | O_TRUNC, info.st_mode);
#endif

    if (new == -1) {

        close(old);

        Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                | DOS9_PRINT_C_ERROR, file, 0);


        return DOS9_UNABLE_COPY;

    }

    while ((count = read(old, buf, sizeof(buf))) != 0) {

        if (count == -1) {


            close(old);
            close(new);

            Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                    | DOS9_PRINT_C_ERROR, file, 0);

            return DOS9_UNABLE_COPY;

        }

        writen = 0;

        while ((writen += write(new, buf + writen, count - writen)) != count) {

            if (writen == -1) {

                close(old);
                close(new);

                Dos9_ShowErrorMessage(DOS9_UNABLE_COPY
                                        | DOS9_PRINT_C_ERROR, file, 0);

                return DOS9_UNABLE_COPY;

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

    return DOS9_NO_ERROR;

}

int Dos9_CatFile(const char* file, const char* dest, int* flags)
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

        Dos9_ShowErrorMessage(DOS9_UNABLE_CAT
                                | DOS9_PRINT_C_ERROR, file, 0);


        return DOS9_UNABLE_CAT;

    }

#ifdef WIN32
    attr = O_WRONLY | O_BINARY | O_CREAT ;
#else
    attr = O_WRONLY | O_CREAT ;
#endif

    if (*flags & DOS9_COPY_CAT_2ND) {

        attr |= O_APPEND;

    } else {

        *flags |= DOS9_COPY_CAT_2ND;
        attr |= O_TRUNC;

    }

    new = open(dest, attr, info.st_mode);

    if (new == -1) {

        close(old);

        Dos9_ShowErrorMessage(DOS9_UNABLE_CAT
                                | DOS9_PRINT_C_ERROR, file, 0);


        return DOS9_UNABLE_CAT;

    }

    while ((count = read(old, buf, sizeof(buf))) != 0) {

        if (count == -1) {


            close(old);
            close(new);

            Dos9_ShowErrorMessage(DOS9_UNABLE_CAT
                                    | DOS9_PRINT_C_ERROR, file, 0);

            return DOS9_UNABLE_CAT;

        }

        writen = 0;

        while ((writen += write(new, buf + writen, count - writen)) != count) {

            if (writen == -1) {

                close(old);
                close(new);

                Dos9_ShowErrorMessage(DOS9_UNABLE_CAT
                                        | DOS9_PRINT_C_ERROR, file, 0);

                return DOS9_UNABLE_CAT;

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

    return DOS9_NO_ERROR;

}
