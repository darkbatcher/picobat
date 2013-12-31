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

int Hlp_StringConvert(iconv_t cd, ESTR* lpEsReturn, char* lpToken)
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
