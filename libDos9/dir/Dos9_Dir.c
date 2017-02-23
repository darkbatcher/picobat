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
#include "../libDos9.h"
#include "../libDos9-int.h"
#include "../../config.h"

#ifdef DOS9_USE_LIBCU8
#include <libcu8.h>
#endif // DOS9_USE_LIBCU8

struct match_args_t {
    int flags;
    FILELIST* files;
    void (*callback)(FILELIST*);
};

static int __inline__ Dos9_IsRegExpTrivial(char* exp)
{
    if (strpbrk(exp, "*?")) {
        return 0;
    } else {
        return 1;
    }
}

static FILELIST* Dos9_AddMatch(char* name, FILELIST* files, struct match_args_t* arg)
{
    FILELIST *file,
             block;

    if (arg->callback) {

        snprintf(block.lpFileName, FILENAME_MAX, "%s", name);

        if (!(arg->flags & DOS9_SEARCH_NO_STAT)) {
            stat(name, &(block.stFileStats));

#ifdef WIN32

            /* Using windows, more detailled directory information
               can be obtained */
            block.stFileStats.st_mode=Dos9_GetFileAttributes(name);

#endif // WIN32
        }

        arg->callback(&block);

        return (((void*)files)+1);

    }

    if ((file = malloc(sizeof(FILELIST))) == NULL)
        return NULL;

    snprintf(file->lpFileName, FILENAME_MAX, "%s", name);

    if (!(arg->flags & DOS9_SEARCH_NO_STAT)) {
        stat(name, &(file->stFileStats));

#ifdef WIN32

        /* Using windows, more detailled directory information
           can be obtained */
        file->stFileStats.st_mode=Dos9_GetFileAttributes(name);

#endif // WIN32
    }

    file->lpflNext = files;

    /* Well, this turns out to produce heaps */
    return file;
}

static FILELIST* Dos9_GetMatch(char* base, char* up, struct match_args_t* arg)
{
    FILELIST *ret = arg->files, *tmp;

    ESTR* path = NULL;
    char *item, *cleanup = NULL;

    DIR* dir;
    struct dirent* ent;

    /* printf("Called Dos9_GetMatch(%s, %s, arg)\n", base, up); */


    /* if something has already been found ... */
    if ((ret != (FILELIST*)-1) && (ret != NULL)
        && (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH))
        return ret;

    if (base == NULL) {
        /* This is somewhat the top-level instance of Dos9_GetMatch ...*/

        if ((arg->flags &
                (DOS9_SEARCH_DIR_MODE | DOS9_SEARCH_RECURSIVE))
            && Dos9_DirExists(up)) {
            /* This regular expression is trivial however either dir mode
               or recursive modes have been specified. Thus we need to
               search matching files _inside_ the directory */

            return Dos9_GetMatch(up, "*", arg);

        }

        if (Dos9_DirExists(up) || Dos9_FileExists(up)) {
            /* this regular expression is trivial (indeed 'up' is the only
               matching file or directory */

            if ((tmp = Dos9_AddMatch(up, ret, arg)) == NULL)
                goto err;

            return tmp;

        } else if (Dos9_IsRegExpTrivial(up)) {
            /* We did not find anything but it is trivial though so there
               is no matching file */
            return NULL;
        }

    }

    /* we will need this */
    path = Dos9_EsInit();

    item = up;

    /* search for the next item in the search in up */
    if (up == NULL) {

       item = "*";

    } else if ((up = strpbrk(up, "\\/"))) {

        cleanup = up;
        *cleanup = '\0';
        up ++;

    }

    /* Check if the item is trivial before really trying
       to recurse in base folder ... */
    if (base != NULL) {

        Dos9_EsCpy(path, base);
        Dos9_EsCat(path, "/");
        Dos9_EsCat(path, item);

    } else {

        Dos9_EsCpy(path, item);

    }



    if (Dos9_FileExists(path->str)) {
        /* if path corresponds to a file (ie. not a dir), there is two
           possibilities (a) up is NULL and then the files matches sinces
           we already reached top level, or (b) up is not NULL and then
           the file cannot match and it wise to stop search in this
           folder */
        if (up == NULL) {

            if ((tmp = Dos9_AddMatch(path->str, ret, arg)) == NULL)
                goto err;

            ret = tmp;
            goto end;

        } else {

            /*abort search */
            goto end;

        }
    }

    if (Dos9_DirExists(path->str)) {
        /* if path corresponds to a dir, always continue search. But path
           is not necessarily to be added to results... ! */

        if (up == NULL) {

            /* add the file and browse if recursive. If the returns uses
               callback, do not forget to add match first. */
            if (arg->callback != NULL) {
                if ((tmp = Dos9_AddMatch(path->str, ret, arg)) == NULL)
                    goto err;

                ret = tmp;
            }

            if (arg->flags & DOS9_SEARCH_RECURSIVE) {

                arg->files = ret;
                ret = Dos9_GetMatch(path->str, up, arg);

            }

            if (arg->callback == NULL) {
                if ((tmp = Dos9_AddMatch(path->str, ret, arg)) == NULL)
                    goto err;

                ret = tmp;
            }

        } else {

            /* always browse */
            arg->files = ret;
            ret = Dos9_GetMatch(path->str, up, arg);

        }

        goto end;

    }

    /* path->str is trivial but it is neither a dir or a file... We can
       be sure we won't get any matching file */
    if (Dos9_IsRegExpTrivial(path->str))
        goto end;

    /* printf("Opening dir=\"%s\" up=\"%s\" item=\"%s\"\n", base, up, item); */

    /* Now we have checked every possible trivial dir, browse dir */
    if ((dir = opendir((base != NULL) ? (base) : ("."))) == NULL)
        goto end;

    /* loop through the directory entities */
    while (ent = readdir(dir)) {

        /* printf("ent : %s\n", ent->d_name);
        getch(); */

        /* skip basic pseudo dirs */
        if ((arg->flags & DOS9_SEARCH_NO_PSEUDO_DIR)
            && (!strcmp(ent->d_name, ".") || !strcmp(ent->d_name, "..")))
            continue;

        /* if the file name definitely matches the expression */
        if (Dos9_RegExpCaseMatch(item, ent->d_name)) {

            /* Compute the path of the current matching entity */
            if (base != NULL) {
                Dos9_EsCpy(path, base);
                Dos9_EsCat(path, "/");
                Dos9_EsCat(path, ent->d_name);
            } else {
                Dos9_EsCpy(path, ent->d_name);
            }

            if (Dos9_FileExists(path->str)) {
                /* The entity is a file, so only add if up is NULL */

                if (up == NULL) {

                    if ((tmp = Dos9_AddMatch(path->str, ret, arg)) == NULL)
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
                    if ((arg->callback != NULL)) {
                        if ((tmp = Dos9_AddMatch(path->str, ret, arg)) == NULL)
                            goto err;

                        ret = tmp;

                        /* If the user only requested the first match */
                        if (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)
                            goto end;
                    }

                    if ((arg->flags & DOS9_SEARCH_RECURSIVE)
                        && (strcmp(ent->d_name, ".") && strcmp(ent->d_name, ".."))) {

                        arg->files = ret;
                        ret = Dos9_GetMatch(path->str, up, arg);

                        if ((ret == -1)
                            || ((ret != (FILELIST*)-1) && (ret != NULL)
                                && (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)))
                            goto end;

                    }

                    if (arg->callback == NULL) {
                        if ((tmp = Dos9_AddMatch(path->str, ret, arg)) == NULL)
                            goto err;

                        ret = tmp;

                        /* If the user only requested the first match */
                        if (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)
                            goto end;
                    }

                } else {

                    /* always browse */
                    arg->files = ret;
                    ret = Dos9_GetMatch(path->str, up, arg);

                    /* exit on error or if the user only requested the first
                       match */
                    if ((ret == -1)
                        || ((ret != (FILELIST*)-1) && (ret != NULL)
                            && (arg->flags & DOS9_SEARCH_GET_FIRST_MATCH)))
                        goto end;

                }

            }

        }

    }

end:
    if (dir)
        closedir(dir);

    if (cleanup)
        *cleanup = '/';

    Dos9_EsFree(path);
    return ret;

err:
    if (dir)
        closedir(dir);

    if (ret)
        Dos9_FreeFileList(ret);

    if (path)
        Dos9_EsFree(path);

    return (void*)-1;
}


LIBDOS9 LPFILELIST  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag)
{
    struct match_args_t args;
    FILELIST* file;

    args.callback = NULL;
    args.flags = iFlag;
    args.files =  NULL;

    file = Dos9_GetMatch(NULL, lpPathMatch, &args);

    if (file == (FILELIST*)-1) {

        fprintf(stderr, "Fatal ERROR : file search\n");
        exit(-1);

    }

    return file;
}

LIBDOS9 int Dos9_GetMatchFileCallback(char* lpPathMatch, int iFlag, void(*pCallBack)(FILELIST*))
{
    struct match_args_t args;
    FILELIST* file;

    args.callback = pCallBack;
    args.flags = iFlag;
    args.files =  NULL;

    file = Dos9_GetMatch(NULL, lpPathMatch, &args);

    if (file == (FILELIST*)-1) {

        fprintf(stderr, "Fatal ERROR : file search\n");
        exit(-1);

    }

    return (int)file;
}



LIBDOS9 char* Dos9_SeekPattern(const char* match, const char* pattern, size_t len)
{
    const char* tok;
    int   i = 0;

    //printf("Looking for pattern \"%s\"[%d] in\"%s\"\n", pattern, len, match);

    while (*match) {

        i = 0;

    //    printf("\tAt \"%s\"\n", match);

        while ((i < len) && (match[i] == pattern[i]))
            i ++;

        if (len == i) {
    //        printf("OK\n");
            return match;
        }
        match ++;
    }

    //printf("FAIL\n");
    return NULL;
}

LIBDOS9 int Dos9_EndWithPattern(const char* match, const char* pattern, size_t len)
{
    size_t size = strlen(match);

    //printf("Comparing \"%s\" and \"%s\" with len %d[%d]...\n", match, pattern, len, size);

    if (size < len)
        return 0;

    match += size - len;

    //printf("====\"%s\" and \"%s\"\n", match, pattern);

    return !strcmp(match, pattern);

}

LIBDOS9 int Dos9_RegExpMatch(const char* regexp, const char* match)
{
	char* next;
    size_t size;

    //printf("*** Comparing \"%s\" et \"%s\"\n", regexp, match);

    if (!(regexp && match)) return FALSE;

    while (*regexp && *match) {

        switch (*regexp) {

            case '?':
                break;

            case '*':
                while (*regexp == '*' || *regexp == '?')
                    regexp ++;

                if (*regexp == '\0')
                    return 1;

                if (next = strpbrk(regexp, "*?")) {

                    size = next - regexp;

                    if (match = Dos9_SeekPattern(match, regexp, size)) {

                        //printf("Match found\n");

                        regexp += size;
                        match += size;

                        //printf("Next : \"%s\" and \"%s\"\n", match, regexp);

                        continue;

                    } else {

                        //printf("Failed\n");

                        return 0;

                    }

                } else {

                    //printf("Checking final point\n");

                    size = strlen(regexp);
                    return Dos9_EndWithPattern(match, regexp, size);

                }

                break;

            default:
                if (*regexp  != *match)
                    return 0;
        }

        regexp = Dos9_GetNextChar(regexp);
        match = Dos9_GetNextChar(match);

    }

    if (*match || *regexp)
        return 0;

    //printf ("*** RETURN : OK\n");

    return 1;

}

LIBDOS9 char* Dos9_SeekCasePattern(const char* match, const char* pattern, size_t len)
{
    const char* tok;
    int   i = 0;

    //printf("Looking for pattern \"%s\"[%d] in\"%s\"\n", pattern, len, match);

    while (*match) {

        i = 0;

    //    printf("\tAt \"%s\"\n", match);

        while ((i < len) && (toupper(match[i]) == toupper(pattern[i])))
            i ++;

        if (len == i) {
    //        printf("OK\n");
            return match;
        }
        match ++;
    }

    //printf("FAIL\n");
    return NULL;
}

LIBDOS9 int Dos9_EndWithCasePattern(const char* match, const char* pattern, size_t len)
{
    size_t size = strlen(match);

    //printf("Comparing \"%s\" and \"%s\" with len %d[%d]...\n", match, pattern, len, size);

    if (size < len)
        return 0;

    match += size - len;

    //printf("====\"%s\" and \"%s\"\n", match, pattern);

    return !stricmp(match, pattern);

}

LIBDOS9 int Dos9_RegExpCaseMatch(const char* regexp, const char* match)
{
	char* next;
    size_t size;

    //printf("*** Comparing \"%s\" et \"%s\"\n", regexp, match);

    if (!(regexp && match)) return FALSE;

    while (*regexp && *match) {

        switch (*regexp) {

            case '?':
                break;

            case '*':
                while (*regexp == '*' || *regexp == '?')
                    regexp ++;

                if (*regexp == '\0')
                    return 1;

                if (next = strpbrk(regexp, "*?")) {

                    size = next - regexp;

                    if (match = Dos9_SeekCasePattern(match, regexp, size)) {

                        //printf("Match found\n");

                        regexp += size;
                        match += size;

                        //printf("Next : \"%s\" and \"%s\"\n", match, regexp);

                        continue;

                    } else {

                        //printf("Failed\n");

                        return 0;

                    }

                } else {

                    //printf("Checking final point\n");

                    size = strlen(regexp);
                    return Dos9_EndWithCasePattern(match, regexp, size);

                }

                break;

            default:
                if (toupper(*regexp)  != toupper(*match))
                    return 0;
        }

        regexp = Dos9_GetNextChar(regexp);
        match = Dos9_GetNextChar(match);

    }

    if (*match || *regexp)
        return 0;

    //printf ("*** RETURN : OK\n");

    return 1;

}

LIBDOS9 int Dos9_FormatFileSize(char* lpBuf, int iLength, unsigned int iFileSize)
{
	int i=0, iLastPart=0;
	if (!iFileSize) {
		return (int)strncpy(lpBuf, "", iLength);
	}
	char* lpUnit[]= {"o", "ko", "mo", "go"};
	while (iFileSize>=1000) {
		iLastPart=iFileSize % 1000;
		iFileSize/=1000;
		i++;
	}
	if (iFileSize>=100) return snprintf(lpBuf, iLength, " %d %s", iFileSize, lpUnit[i%4]);
	if (iFileSize>=10) return snprintf(lpBuf, iLength, "  %d %s", iFileSize, lpUnit[i%4]);
	else {
		iLastPart=iLastPart/10;
		return snprintf(lpBuf, iLength, "%d,%.2d %s", iFileSize, iLastPart , lpUnit[i%4]);
	}
}

LIBDOS9 size_t Dos9_GetStaticLength(const char* str)
{
    char *ptr = str,
         *orig = str;

    while (*str) {

        if (*str == '\\' || *str == '/') {

            ptr = str;

        } else if (*str == '*' || *str == '?') {

            break;

        }

        str ++;

    }

    return (size_t)(ptr-orig);
}

int _Dos9_FreeFileList(LPFILELIST lpflFileList)
{
	LPFILELIST lpflNext;

	while (lpflFileList) {

		lpflNext=lpflFileList->lpflNext;
		free(lpflFileList);
        lpflFileList = lpflNext;

	}

	return 0;
}


LIBDOS9 int Dos9_FreeFileList(LPFILELIST lpflFileList)
{

	_Dos9_FreeFileList(lpflFileList);

	return 0;

}

#if defined(WIN32) && defined(DOS9_USE_LIBCU8)
#include <libcu8.h>

int Dos9_GetFileAttributes(const char* file)
{
    wchar_t *wfile;
    int ret;
    size_t conv;

    if (!(wfile= libcu8_xconvert(LIBCU8_TO_U16, file,
                                        strlen(file)+1, &conv)))
        return -1;

    ret = GetFileAttributesW(wfile);

    free(wfile);

    return ret;
}

#elif defined(WIN32) && !defined(DOS9_USE_LIBCU8)
int Dos9_GetFileAttributes(const char* file)
{
    return GetFileAttributes(file);
}
#endif // defined
