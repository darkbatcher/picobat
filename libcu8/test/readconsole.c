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

void fill_input(wchar_t* wcs, INPUT* input_buf, size_t len)
{
    while (len) {

        input_buf->type = INPUT_KEYBOARD;
        input_buf->ki.wVk = 0;
        input_buf->ki.wScan = *wcs;
        input_buf->ki.dwFlags = KEYEVENTF_UNICODE;
        input_buf->ki.time = 0;
        input_buf->ki.dwExtraInfo = 0;

        input_buf++;
        wcs ++;
        len --;

    }
}

/* Readconsole -- Check libcu8 rightly read unicode input from
   the console */
int main(void)
{
    wchar_t input_string[] = L"This is a test\xE9 phrase\r";
    char input_u8[]="This is a test\xC3\xA9 phrase\n";
    INPUT input_buffer[NB_ITEMS(input_string)-1];
    HANDLE old;

    int size, ret;
    char buf[FILENAME_MAX],
        buf2[FILENAME_MAX];

    if (libcu8_init(NULL)) {
        log_msg("libcu8_init() failed");
        return 1;
    }


    fill_input(input_string, input_buffer, NB_ITEMS(input_buffer));

    old = GetForegroundWindow();

    SetForegroundWindow(GetConsoleWindow());

    SendInput(NB_ITEMS(input_buffer), input_buffer, sizeof(INPUT));

    SetForegroundWindow(old);

    if (fgets(buf, sizeof(buf), stdin) == NULL
        || strlen(buf) != sizeof(input_u8)-1) {
        log_msg("read() returned wrong size.\n");
        return 1;
    } else if (strncmp(buf, input_u8, sizeof(input_u8)-1)) {
        log_msg("read() returned invalid translation\n");
        return 1;
    }

    return 0;
}
