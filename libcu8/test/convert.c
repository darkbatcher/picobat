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

#include <libcu8.h>

#include "log_msg.h"

/* Test that the encoding function libcu8_xconvert work. We do not test
   whether very specific characters are getting translated the right
   way (IMO, this should be tested by libiconv). Just check that simple
   text get translated the right way*/
int main(void)
{
    /* define two texts, one in utf-8 and the other in utf16 */
    char u8_lorem[] = "Lorem ipsum dolor sit amet, consectetur adipiscing "
    "elit. Nullam vitae quam eros. Ut non risus enim. Aenean ornare augue at "
    "turpisdictum porta. Aenean ut sapien vel nunc viverra fringilla vitae eu "
    "quam. Sed sit amet magna pulvinar, malesuada elit et, imperdiet nunc. "
    "Sed vestibulum mollis semper. Integer in sodales purus. Morbi mattis "
    "turpis erat, eget accumsan odio varius ac. Sed eget vulputate magna. "
    "Vivamus ac sapien vitae justo elementum accumsan. Proin non odio in sem "
    "rhoncus bibendum. Donec interdum risus aliquam, consectetur lectus "
    "facilisis, hendrerit libero. Suspendisse potenti.";
    wchar_t u16_lorem[] = L"Lorem ipsum dolor sit amet, consectetur adipiscing "
    L"elit. Nullam vitae quam eros. Ut non risus enim. Aenean ornare augue at "
    L"turpisdictum porta. Aenean ut sapien vel nunc viverra fringilla vitae eu "
    L"quam. Sed sit amet magna pulvinar, malesuada elit et, imperdiet nunc. "
    L"Sed vestibulum mollis semper. Integer in sodales purus. Morbi mattis "
    L"turpis erat, eget accumsan odio varius ac. Sed eget vulputate magna. "
    L"Vivamus ac sapien vitae justo elementum accumsan. Proin non odio in sem "
    L"rhoncus bibendum. Donec interdum risus aliquam, consectetur lectus "
    L"facilisis, hendrerit libero. Suspendisse potenti.";

    wchar_t *trans;
    size_t retsize;

    int i;

    /* we don't need init the libcu8 library */
    //libcu8_init();

    /* Try to convert u8_lorem to utf16 */
    trans = (wchar_t*) libcu8_xconvert(LIBCU8_TO_U16, u8_lorem,
                                            sizeof(u8_lorem), &retsize);

    if (trans == NULL) {
        log_msg("libcu8_xconvert() returned null pointer");
        return -1;
    }

    if (retsize != sizeof(u16_lorem)) {
        log_msg("libcu8_xconvert() returned wrong size");
        return 0;
    }


    if(memcmp(trans, u16_lorem, retsize)) {
        log_msg("libcu8_convert() returned wrong translation");
        return -1;
    }

    free(trans);

    return 0;
}
