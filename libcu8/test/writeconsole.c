/*

 libcu8 - A wrapper to fix msvcrt utf8 incompatibilities issues
 Copyright (c) 2014, 2015, 2016 Romain GARBI (Darkbatcher)

 All rights reserved.
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
 * Neither the name of the name of Romain Garbi (Darkbatcher) nor the
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
#include <string.h>
#include <wchar.h>
#include <io.h>
#include <fcntl.h>
#include <windows.h>
#include <libcu8.h>

#include "log_msg.h"

#define NB_ITEMS(data) (sizeof(data)/sizeof(data[0]))


/* Write console -- Check basic output to console */
int main(void)
{
    char buf[]="This is a test\xC3\xA9 phrase";
    int sz;
    CONSOLE_SCREEN_BUFFER_INFO info;
    HANDLE out;

    if (libcu8_init(NULL) == -1) {
        log_msg("writeconsole: impossible to init libcu8");
        return 1;
    }

    if ((out = GetStdHandle(STD_OUTPUT_HANDLE))
                            == INVALID_HANDLE_VALUE) {
        log_msg("writeconsole: impossible to grab output handle");
        return 1;
    }

    GetConsoleScreenBufferInfo(out, &info);

    sz = info.dwCursorPosition.X;

    if (fputs(buf, stdout) == EOF) {
        log_msg("writeconsole: failed to fputs() to stdout");
        return 1;
    }

    GetConsoleScreenBufferInfo(out, &info);

    /* count written characters based on cursor position */
    sz = info.dwCursorPosition.X - sz;

    if (sz + 1!= strlen(buf)) {
        log_msg("writeconsole: written incorrect amount of characters");
        return 1;
    }

    return 0;
}
