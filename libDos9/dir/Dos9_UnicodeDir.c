/*
 *
 *   libDos9 - The Dos9 project
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


#include <ctype.h>
#include <string.h>
#include <wchar.h>

#include "../libDos9.h"
#include "../libDos9-int.h"
#include "../../config.h"

#if defined(DOS9_USE_LIBCU8)
#include <windows.h>
#include <Shlwapi.h>
#include <libcu8.h>

#define XSIZE(a) sizeof(a)/sizeof(a [0])

struct match_args_t {
    int flags;
    FILELIST* files;
    void (*callback)(FILELIST*);
};

static void xwcsncat(wchar_t* restrict dest, wchar_t* restrict source, size_t size)
{
    /* skip first bytes */
    while (*dest && size > 1) {
        dest ++;
        size --;
    }

    while (*source && size > 1) {
        *dest = *source;
        dest ++;
        source ++;
        size --;
    }

    *dest = '\0';
}

wchar_t* Dos9_GetNextLevel(wchar_t* up, int* jok)
{
    int joker = 0;
    wchar_t* prev = NULL;

    *jok = 0;

    while (*up) {

        switch (*up) {

        case L'*':
        case L'?':
            joker = 1;
            if (prev)
                return prev;
            *jok = 1;
            break;

        case L'/':
        case L'\\':
            if (joker)
                return up;
            prev = up;

        }

        up++;
    }

    return NULL;
}

static int __inline__ Dos9_FileExists_W(const wchar_t* ptrName)
{
    int iAttrib;

    iAttrib = GetFileAttributesW(ptrName);

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            !(iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

static int __inline__ Dos9_DirExists_W(const wchar_t *ptrName)
{
    int iAttrib;

    iAttrib = GetFileAttributesW(ptrName);

    return (iAttrib != INVALID_FILE_ATTRIBUTES &&
            (iAttrib & FILE_ATTRIBUTE_DIRECTORY));
}

static int __inline__ Dos9_IsRegExpTrivial(wchar_t* exp)
{
    if (wcspbrk(exp, L"*?")) {
        return 0;
    } else {
        return 1;
    }
}

static time_t __inline__  Dos9_ToTime_t(FILETIME* ft)
{
    ULARGE_INTEGER ull;
    ull.LowPart = ft->dwLowDateTime;
    ull.HighPart = ft->dwHighDateTime;
    return ull.QuadPart / 10000000ULL - 11644473600ULL;
}

static void __inline__ Dos9_FillStat(struct stat* st, WIN32_FIND_DATAW* data)
{
    st->st_ctime = Dos9_ToTime_t(&(data->ftCreationTime));
    st->st_atime = Dos9_ToTime_t(&(data->ftLastAccessTime));
    st->st_mtime = Dos9_ToTime_t(&(data->ftLastWriteTime));

    st->st_mode = data->dwFileAttributes;
    st->st_size = ((size_t)data->nFileSizeLow) | (((size_t)data->nFileSizeHigh) << sizeof(DWORD));
}

static void __inline__ Dos9_GetStat(struct stat* st, wchar_t* wname)
{
    WIN32_FILE_ATTRIBUTE_DATA attr;

    if (GetFileAttributesExW(wname, GetFileExInfoStandard, &attr) == 0)
        return;

    st->st_ctime = Dos9_ToTime_t(&(attr.ftCreationTime));
    st->st_atime = Dos9_ToTime_t(&(attr.ftLastAccessTime));
    st->st_mtime = Dos9_ToTime_t(&(attr.ftLastWriteTime));

    st->st_mode = attr.dwFileAttributes;
    st->st_size = ((size_t)attr.nFileSizeLow) | (((size_t)attr.nFileSizeHigh) << sizeof(DWORD));
}

static FILELIST* Dos9_AddMatch(wchar_t* wname, FILELIST* files, struct match_args_t* arg, WIN32_FIND_DATAW* data)
{
    FILELIST *file,
             block;
    char *name;
    size_t cvt;

    if (!(name = libcu8_xconvert(LIBCU8_FROM_U16, wname,
                                    (wcslen(wname)+1)*sizeof(wchar_t), &cvt)))
        goto err;

    if (arg->callback) {

        snprintf(block.lpFileName, FILENAME_MAX, "%s", name);

        if (!(arg->flags & DOS9_SEARCH_NO_STAT)) {
            /* Using windows, more detailed directory information
               can be obtained */
            if (data == NULL) {

                Dos9_GetStat(&(block.stFileStats), wname);

            } else {

                Dos9_FillStat(&(block.stFileStats), data);

            }

        }

        arg->callback(&block);

        free(name);

        return (((void*)files)+1);

    }

    if ((file = malloc(sizeof(FILELIST))) == NULL)
        goto err;

    strncpy(file->lpFileName, name, FILENAME_MAX);
    file->lpFileName[FILENAME_MAX - 1] = '\0';

    /* if the program requested stat infos */
    if (!(arg->flags & DOS9_SEARCH_NO_STAT)) {
        if (data == NULL) {

            Dos9_GetStat(&(file->stFileStats), wname);

        } else {

            Dos9_FillStat(&(file->stFileStats), data);

        }
    }

    if (files)
        files->lpflNext = file;

    file->lpflNext = NULL;
    file->lpflPrevious = files;

    free(name);

    return file;

err:
    if (name)
        free(name);

    return NULL;
}

static FILELIST* Dos9_GetMatch(wchar_t* restrict base, wchar_t* restrict up, struct match_args_t* arg)
{
    FILELIST *ret = arg->files, *tmp;

    wchar_t path[FILENAME_MAX];
    wchar_t *item, *cleanup = NULL;

    HANDLE dir = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATAW ent;

    int loop, joker;

    /* if something has already been found ... */
    if ((ret != (FILELIST*)-1) && (ret != NULL)
        && (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH))
        return ret;

    if (base == NULL) {
        /* This is somewhat the top-level instance of Dos9_GetMatch ...*/

        if ((arg->flags &
                (DOS9_SEARCH_DIR_MODE | DOS9_SEARCH_RECURSIVE))
            && Dos9_DirExists_W(up)) {
            /* This regular expression is trivial however either dir mode
               or recursive modes have been specified. Thus we need to
               search matching files _inside_ the directory */

            /* First, check that the name does not end with a slash, if it
               is the case, remove it */
            if (((item = wcsrchr(up, L'\\')) && (*(item+1) == L'\0'))
                || ((item = wcsrchr(up, '/')) && (*(item+1) == L'\0')))
                *item = L'\0';

            if (!(arg->flags & DOS9_SEARCH_DIR_MODE)
                && (arg->callback != NULL)) {
                /* If dir mode is not specified, add that match too */
                if ((tmp = Dos9_AddMatch(up, ret, arg, NULL)) == NULL)
                    goto err;

                arg->files = tmp;
            }

            ret = Dos9_GetMatch(up, L"*", arg);

            if (!(arg->flags & DOS9_SEARCH_DIR_MODE)
                && arg->callback == NULL) {
                if ((tmp = Dos9_AddMatch(up, ret, arg, NULL)) == NULL)
                    goto err;

                ret = tmp;
            }

            goto end;

        }

        if (Dos9_DirExists_W(up) || Dos9_FileExists_W(up)) {
            /* this regular expression is trivial (indeed 'up' is the only
               matching file or directory */

            if ((tmp = Dos9_AddMatch(up, ret, arg, NULL)) == NULL)
                goto err;

            return tmp;

        } else if (Dos9_IsRegExpTrivial(up)) {
            /* We did not find anything but it is trivial though so there
               is no matching file */
            return NULL;
        }

    }

    item = up;

    /* search for the next item in the search in up */
    if (up == NULL) {

       item = L"*";
       joker = 1;

    } else if ((up = Dos9_GetNextLevel(up, &joker))) {

        cleanup = up;
        *cleanup = L'\0';
        up ++;

    }

    if (!joker) {
        /* Check if the item is trivial before really trying
           to recurse in base folder ... */
        if (base != NULL) {

            wcsncpy(path, base, XSIZE(path));
            xwcsncat(path, L"\\", XSIZE(path));
            xwcsncat(path, item, XSIZE(path));

        } else {

            wcsncpy(path, item, XSIZE(path));
            path[XSIZE(path) - 1] = '\0';

        }

        if (Dos9_FileExists_W(path)) {

            /* if path corresponds to a file (ie. not a dir), there is two
               possibilities (a) up is NULL and then the files matches since
               we already reached top level, or (b) up is not NULL and then
               the file cannot match and it is wise to stop search in this
               folder */
            if (up == NULL) {

                if ((tmp = Dos9_AddMatch(path, ret, arg, NULL)) == NULL)
                    goto err;

                ret = tmp;
                goto end;

            }

        }

        if (Dos9_DirExists_W(path)) {

            /* if path corresponds to a dir, always continue search. But path
               is not necessarily to be added to results... ! */
            if (up == NULL) {

                /* add the file and browse if recursive. If the returns uses
                   callback, do not forget to add match first. */
                if (arg->callback != NULL) {
                    if ((tmp = Dos9_AddMatch(path, ret, arg, NULL)) == NULL)
                        goto err;

                    ret = tmp;
                }

                if (arg->flags & DOS9_SEARCH_RECURSIVE) {

                    arg->files = ret;
                    ret = Dos9_GetMatch(path, up, arg);

                }

                if (arg->callback == NULL) {
                    if ((tmp = Dos9_AddMatch(path, ret, arg, NULL)) == NULL)
                        goto err;

                    ret = tmp;
                }

            } else {

                /* always browse */
                arg->files = ret;
                ret = Dos9_GetMatch(path, up, arg);

            }

            goto end;

        }


        /* path->str is trivial but it is neither a dir or a file... We can
           bet we won't get any matching file */
        goto end;

    }

    if (base != NULL) {

        /* we could do better than that, in case of non recursive search,
           but we won't, because various versions of windows result in
           various regular expression interpretation, as such we may
           prefer to use our custom functions when it is needed */

        wcsncpy(path, base, XSIZE(path));
        xwcsncat(path, L"\\*", XSIZE(path));

    } else {

        *path = L'*';
        *(path + 1) = L'\0';

    }

    if ((dir = FindFirstFileW(path, &ent)) == INVALID_HANDLE_VALUE)
        goto end;

    loop = 1;

    /* loop through the directory entities */
    while (loop) {

        /* skip basic pseudo dirs */
        if ((arg->flags & DOS9_SEARCH_NO_PSEUDO_DIR)
            && (!wcscmp(ent.cFileName, L".") || !wcscmp(ent.cFileName, L".."))) {
            loop = FindNextFileW(dir, &ent);
            continue;
        }

        /* Compute the path of the current matching entity */
        if (base != NULL) {

            wcsncpy(path, base, XSIZE(path));
            xwcsncat(path, L"\\", XSIZE(path));
            xwcsncat(path, ent.cFileName, XSIZE(path));

        } else {

            wcsncpy(path, ent.cFileName, XSIZE(path));
            path[XSIZE(path) - 1] = '\0';

        }

        if (!(ent.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            && PathMatchSpecW(ent.cFileName, item)) {
            /* The entity is a file, so only add if up is NULL */

            if (up == NULL) {

                if ((tmp = Dos9_AddMatch(path, ret, arg, &ent)) == NULL)
                    goto err;

                ret = tmp;

                if (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)
                        goto end;

            }

        } else {
            /* the entity is a directory, browse if (a) up is not NULL
            or (b) the search is recursive and up is NULL. Only add if
            up is NULL*/
            if (up == NULL) {

                /* add the file and browse if recursive. If the returns uses
                   callback, do not forget to add match first. */
                if ((arg->callback != NULL)
                    && PathMatchSpecW(ent.cFileName, item)) {
                    if ((tmp = Dos9_AddMatch(path, ret, arg, &ent)) == NULL)
                        goto err;

                    ret = tmp;

                    /* If the user only requested the first match */
                    if (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)
                        goto end;
                }

                if ((arg->flags & DOS9_SEARCH_RECURSIVE)
                    && (wcscmp(ent.cFileName, L".") && wcscmp(ent.cFileName, L".."))) {

                    arg->files = ret;
                    ret = Dos9_GetMatch(path, item, arg);

                    if ((ret == (FILELIST*)-1)
                        || ((ret != (FILELIST*)-1) && (ret != NULL)
                            && (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)))
                        goto end;

                }

                if ((arg->callback == NULL)
                    && PathMatchSpecW(ent.cFileName, item)) {
                    if ((tmp = Dos9_AddMatch(path, ret, arg, &ent)) == NULL)
                        goto err;

                    ret = tmp;

                    /* If the user only requested the first match */
                    if (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)
                        goto end;
                }

            } else {

                /* always browse */
                arg->files = ret;
                ret = Dos9_GetMatch(path, up, arg);

                /* exit on error or if the user only requested the first
                   match */
                if ((ret == (FILELIST*)-1)
                    || ((ret != (FILELIST*)-1) && (ret != NULL)
                        && (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)))
                    goto end;

            }

        }

        /* Get the next file */
        loop = FindNextFileW(dir, &ent);

    }

end:
    if (dir != INVALID_HANDLE_VALUE)
        FindClose(dir);

    if (cleanup)
        *cleanup = L'/';

    return ret;

err:
    if (dir != INVALID_HANDLE_VALUE)
        FindClose(dir);

    if (ret)
        Dos9_FreeFileList(ret);

    return (void*)-1;
}

LIBDOS9 LPFILELIST  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag)
{
    struct match_args_t args;
    FILELIST* file;
    wchar_t *wpath;
    size_t cvt;

    if (!(wpath = libcu8_xconvert(LIBCU8_TO_U16, lpPathMatch,
                                        strlen(lpPathMatch)+1, &cvt)))
        return NULL;


    args.callback = NULL;
    args.flags = iFlag;
    args.files =  NULL;

    file = Dos9_GetMatch(NULL, wpath, &args);

    if (file == (FILELIST*)-1) {

        fprintf(stderr, "Fatal ERROR : file search\n");
        exit(-1);

    }

    free(wpath);

    if (file)
        while (file->lpflPrevious)
            file = file->lpflPrevious;

    return file;
}

LIBDOS9 int Dos9_GetMatchFileCallback(char* lpPathMatch, int iFlag, void(*pCallBack)(FILELIST*))
{
    struct match_args_t args;
    FILELIST* file;
    wchar_t *wpath;
    size_t cvt;

    if (!(wpath = libcu8_xconvert(LIBCU8_TO_U16, lpPathMatch,
                                        strlen(lpPathMatch)+1, &cvt)))
        return NULL;


    args.callback = pCallBack;
    args.flags = iFlag;
    args.files =  NULL;

    file = Dos9_GetMatch(NULL, wpath, &args);

    if (file == (FILELIST*)-1) {

        fprintf(stderr, "Fatal ERROR : file search\n");
        exit(-1);

    }

    free(wpath);

    return (int)file;
}

#endif
