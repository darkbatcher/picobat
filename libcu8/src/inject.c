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
#include <stdio.h>
#include <stdlib.h>
/* #include <wchar.h> */
/* #include <io.h> */
#include <fcntl.h>
#include <errno.h>
#include <windows.h>

// #include "internals.h"
/* include "libcu8.h" */

static HANDLE self = INVALID_HANDLE_VALUE;

/* A pointer array that hold absolute address of libcu8 functions
   near the actual location of msvcrt functions, so that relative
   addressing can be used  */
static void** fn_tbl = NULL;

#define BASE_MASK 0xffffffffff000000

/* Reserve memory for the array of pointers */
int libcu8_reserve_fn_table(int nb, void* base_fn)
{
    /* compute a base address for fn_tbl, close enough to be in
       2GB range around the original function position  */
    void *base = (void*)((INT64)_read & BASE_MASK),
         *addr;
    int size;
    SYSTEM_INFO sysinfo;

    /* get basic informations about the OS */
    GetSystemInfo(&sysinfo);

    /* Compute the number of pages to reserve */
    size = ((nb * sizeof(void*) + sysinfo.dwPageSize) / sysinfo.dwPageSize)
                * sysinfo.dwPageSize;

    addr = ((char*)base +  (~BASE_MASK / sysinfo.dwPageSize)
                            * sysinfo.dwPageSize );

    while (addr >= base) {

        //printf("Trying to reserve address %p ...\n", addr);

        if ((fn_tbl = VirtualAlloc(addr, size, MEM_RESERVE | MEM_COMMIT,
                                                       PAGE_EXECUTE_READWRITE)))
            break;

        addr = ((char*)addr - sysinfo.dwPageSize);

    }

    if (fn_tbl == NULL)
        return -1;

    //printf("fn_tbl = %p\n", fn_tbl);

    return 0;
}

void** libcu8_get_fn_pointer(void* fn)
{
    void** ret;
    if (!fn_tbl)
        return NULL;

    *fn_tbl = fn;
    //printf("Pointer at %p : %p", fn_tbl , fn);
    ret = fn_tbl ++;

    return ret;
}


/* Replace oldfn and newfn for the entire process at runtime.
   Note: We have to be careful and check that oldfn isn't used when
   replacing it */
int __cdecl libcu8_replace_fn(void* oldfn, void* newfn, int n)
{
    /* These inject code for x86_64 or i386 are derived from asm code of the
       kind of :

            jmp [rel32]

       Where rel32 obviously refer to the relative location of a function
       pointer stored in tbl_fn.

       Note there is another constraint, the sequence will be inserted within
       msvcrt function, so it needs to be really short, otherwise it won't
       fit and subsequent replacement may overwrite parts of it.

       These codes have the advantage to be simple and not to affect most of the
       calling conventions on windows, such as __cdecl, __stdcall, __fastcall
       or so.

        The code[] array contains opcode for the following assembler mnemonics

                x86_64 mode            |               i386 mode
                                       |
            jmp [rel32]                |          jmp addr32


        This mnemonics are fairly interesting because they result in *exactly*
        the same binary code, simplifying the code greatly

    */
    unsigned char code[]={0xff, 0x25, 0x00, 0x00, 0x00, 0x00};
    size_t written;
    int ret;
    void** tbl_fn;
    int rel;

    if ((self == INVALID_HANDLE_VALUE
        && (self = GetCurrentProcess()) == NULL)
        || (fn_tbl == NULL &&
            libcu8_reserve_fn_table(n, oldfn) == -1 ))
        return 1;


    if ((tbl_fn = libcu8_get_fn_pointer(newfn)) == NULL)
        return 1;

#if defined(__x86_64__)
    rel = (void*)tbl_fn - oldfn; /* compute relative adress */
    rel -= sizeof(code);
#else
    rel = tbl_fn; /* compute absolute address */
#endif

    //printf("newfn = %p, diff = %p - %p :  %X\n", newfn, tbl_fn, oldfn, rel);
    //printf("_read = %p", *_read);
    //printf(" *tbl_fn  = %p\n",  *tbl_fn);

    /* copy the actual function pointer in the inject code */
    memcpy ( code + 2, &rel, sizeof(code)-2);

    //int i;
    //printf("{");
    //for (i=0; i < sizeof(code); i++)
    //    printf("0x%X, ", code[i]);
    //printf("}\n");

    /* put the inject code at the address of the function to
       replace */
    ret = WriteProcessMemory(self, oldfn, code,
                                    sizeof(code), &written);

    //printf("Return %d\n", ret);
    //_read(0,0,0);

    if (!ret || written != sizeof(code))
        return 2;

    CloseHandle(self);

    return 0;
}

void __cdecl libcu8_save_changes(void)
{
    if (self != INVALID_HANDLE_VALUE)
        CloseHandle(self);
}
