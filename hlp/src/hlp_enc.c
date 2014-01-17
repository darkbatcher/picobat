/*
 *
 *   HLP - A free cross platform manual manager - The Dos9 project
 *   Copyright (C) 2010-2014 DarkBatcher
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


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#ifdef WIN32

    #include "libintl.h"
    #include "iconv.h"

#else

    #include <libintl.h>
    #include <iconv.h>

#endif

#include "hlp_load.h"
#include "libDos9.h"

int Hlp_StringConvert(iconv_t cd, ESTR* lpEsReturn, const char* lpToken)
{
    char lpTmpBuf[128]="";
    char *lpTmpToken=lpTmpBuf;
    size_t tmpSize=sizeof(lpTmpBuf);
    size_t originSize=strlen(lpToken);
    size_t nCount;

    Dos9_EsCpy(lpEsReturn, "");

    iconv(cd, NULL, NULL, &lpTmpToken, &tmpSize);

    while (1) {

        nCount = iconv(cd, &lpToken, &originSize, &lpTmpToken, &tmpSize);

        if (nCount == (size_t)-1) {

            switch(errno) {

                case E2BIG: /* not sufficient space */
                    Dos9_EsCpyN(lpEsReturn, lpTmpBuf, sizeof(lpTmpBuf)+1);
                    lpTmpToken=lpTmpBuf;
                    tmpSize=sizeof(lpTmpBuf);
                    break;

                case EILSEQ:
                case EINVAL:
                    HANDLE_ERROR(gettext("HLP :: Error, Encoutered invalid charset sequence:\n"
                                         "%s"), lpToken);
                    return -1;
                    break;


            }

        } else if (originSize==0){

            break;

        }

    }

    Dos9_EsCat(lpEsReturn, lpTmpBuf);

    return 0;

}
