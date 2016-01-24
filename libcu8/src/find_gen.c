/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015, 2016 Romain GARBI

 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of the name of Romain Garbi nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY ROMAIN GARBI AND CONTRIBUTORS ``AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL ROMAIN GARBI AND CONTRIBUTORS BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES
 LOSS OF USE, DATA, OR PROFITS OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#define cat(a, ...)          __cat(a, __VA_ARGS__)
#define __cat(a, ...)        a ## __VA_ARGS__
#define suff(x)              cat(x, __libcu8_find_suffix)
#define __libcu8_findfirst   suff(libcu8_findfirst)
#define __libcu8_findnext    suff(libcu8_findnext)
#define __libcu8_finddata_t  cat(_finddata, cat(__libcu8_find_suffix, _t ))
#define __libcu8_wfinddata_t cat(_wfinddata, cat(__libcu8_find_suffix, _t ))
#define __libcu8_wfindfirst  suff(_wfindfirst)
#define __libcu8_wfindnext   suff(_wfindnext)

__LIBCU8__IMP __cdecl intptr_t __libcu8_findfirst (const char* file,
                                struct __libcu8_finddata_t* inf)
{
    wchar_t *wcs;
    char *name;
    size_t cvt;
    intptr_t handle;
    struct __libcu8_wfinddata_t info;

    if (!(wcs = (wchar_t*) libcu8_xconvert(LIBCU8_TO_U16, file,
                                                strlen(file)+1, &cvt)))
        return -1;

    handle = __libcu8_wfindfirst(wcs, &info);

    if (handle != -1
        && (name = (wchar_t*) libcu8_xconvert(LIBCU8_FROM_U16, info.name,
                                (wcslen(info.name)+1)*sizeof(wchar_t), &cvt))) {

        if (cvt > FILENAME_MAX) {

            errno = EINVAL;
            _findclose(handle);
            handle = -1;

        } else
            strcpy(inf->name, name);

        free(name);

    } else if (handle != -1) {

        errno = ENOMEM;
        _findclose(handle);
        handle = -1;

    }

    free (wcs);
    return handle;
}
__LIBCU8__IMP __cdecl int __libcu8_findnext (intptr_t handle,
                                    struct __libcu8_finddata_t* inf)
{
    char *name;
    size_t cvt;
    int ret;
    struct __libcu8_wfinddata_t info;

    ret = __libcu8_wfindnext (handle, &info);

    if (ret != -1
        && (name = (wchar_t*) libcu8_xconvert(LIBCU8_FROM_U16, info.name,
                                (wcslen(info.name)+1)*sizeof(wchar_t), &cvt))) {

        if (cvt > FILENAME_MAX) {

            errno = EINVAL;
            ret = -1;

        } else
            strcpy(inf->name, name);

        free(name);

    } else if (ret != -1) {

        errno = ENOMEM;
        ret = -1;

    }

    return ret;
}
