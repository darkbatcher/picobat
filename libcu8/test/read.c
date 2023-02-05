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
#include <libcu8.h>

#include "log_msg.h"


#define CHECK_U8_FILE "check_u8"
int main(void)
{
    char check_u8[] = "Proin fam\xC3\xA8s vulputate conubi\xC3\xA9 n\xC3\xBBllam justo mauris "
    "condiment\xC3\xBBm s\xC3\xA8n\xC3\xA8ktus du aenean elit.",
         buf[sizeof(check_u8)];

    char* ret;
    FILE *f;

    if (libcu8_init(NULL)) {
        log_msg("libcu8_init() failed");
        return 1;
    }

    if ((f = fopen(CHECK_U8_FILE, "r")) == NULL) {
        log_msg("Unable to open() check_u8 file");
        return 1;
    }

    ret = fgets(buf, sizeof(buf), f);
    if ( ret == NULL
        || strlen(buf) != sizeof(check_u8)-1) {
        log_msg("read() did not read enough bytes");
        return 1;
    }

    if (memcmp(buf, check_u8, sizeof(check_u8))) {
        log_msg("read() returned wrong content");
        return 1;
    }

    fclose (f);

    return 0;
}
