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


#define REM_SIZE 3

int main(void)
{
    wchar_t check_eilseq[] = L"Illegal char sequence",
            check_einval[] = {L'E', L'i', L'n', L'v', L'a', L'l'},
            *checks[] = {check_eilseq, check_einval},
            *xcvt,
            *cvt;

    char eilseq[] = "Illegal \337char \xC3sequence",
         einval[] = {'E', 'i', 'n', 'v', 'a', 'l', 0xC3},
         *remainders[] = {"", "\xC3"},
         *tests[] = {eilseq, einval, NULL},
         remainder[REM_SIZE];

    size_t ret, xret,
           checks_size[] = {sizeof(check_eilseq), sizeof(check_einval)},
           tests_size[] = {sizeof(eilseq), sizeof(einval)},
           remainders_sizes[] = {0, 1},
           count;

    int i=0;

     if (libcu8_init(NULL)) {
        log_msg("libcu8_init() failed");
        return -1;
    }

    while (tests[i]) {

        if (!(xcvt = (wchar_t*)libcu8_xconvert(LIBCU8_TO_U16, tests[i],
                                            tests_size[i], &xret))) {

            log_msg("libcu8_xconvert() failed");
            return 1;

        }

        count = 0;
        if (!(cvt = (wchar_t*)libcu8_convert(LIBCU8_TO_U16, tests[i],
                                            tests_size[i], remainder,
                                            &count, REM_SIZE, &ret))) {

            log_msg("libcu8_convert() failed");
            return 1;

        }

        if (xret != checks_size[i]) {

            log_msg("converted strings via xconvert sizes do not match");
            return 1;

        }

        if (ret != checks_size[i]) {

            log_msg("converted strings via convert sizes do not match");
            return 1;

        }

        if (count != remainders_sizes[i]) {

            log_msg("converted strings via convert remainders sizes do not match");
            return 1;

        }

        if (memcmp(xcvt, checks[i], xret)) {

            log_msg("converted strings via xconvert do not match");
            return 1;

        }

        if (memcmp(cvt, checks[i], ret)) {

            log_msg("converted strings via convert do not match");
            return 1;

        }

        if ((count != 0)
            && memcmp(remainder, remainders[i], count)) {

            log_msg("remainders obtained via convert do not match");
            return 1;

        }

        i++;

        free(xcvt);
        free(cvt);
    }

    return 0;
}
