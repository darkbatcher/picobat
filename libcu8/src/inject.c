/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015 Romain GARBI

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
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <errno.h>
#include <windows.h>

#include "internals.h"
#include "libcu8.h"

#if defined(__x86_64__)
#define CODE x86_64_code
#define PTR_OFFSET 2
#elif defined(__i386__)
#define CODE i386_code
#define PTR_OFFSET 1
#endif // __x86_64__

/* Replace oldfn and newfn for the entire process at runtime.
   Note: We have to be careful and check that oldfn isn't used when
   replacing it */
int __cdecl libcu8_replace_fn(void* oldfn, void* newfn)
{
    /* These inject code for x86_64 or i386 are derived from asm code of the
       kind of :

            mov %eax, newfn
            jmp %eax

       These codes have the advantage to be simple and not to affect most of the
       calling conventions on windows, such as __cdecl, __stdcall, __fastcall
       or so. */
    unsigned char x86_64_code[]={0x48, 0xb8, 0x00, 0x00, 0x00, 0x00,
                                    0x00, 0x00, 0x00, 0x00, 0xff, 0xe0},
                  i386_code[] = {0xb8, 0x00, 0x00, 0x00, 0x00, 0xff, 0xe0};
    size_t written;
    int ret;
    HANDLE self;

    if ((self = GetCurrentProcess()) == NULL)
        return 1;

    /* copy the actual function pointer in the inject code */
    memcpy ( CODE + PTR_OFFSET, &newfn, sizeof(newfn));

    /* put the inject code at the address of the function to
       replace */
    ret = WriteProcessMemory(self, oldfn, CODE,
                                    sizeof(CODE), &written);

    if (!ret || written != sizeof(CODE))
        return 2;

    CloseHandle(self);

    return 0;
}
