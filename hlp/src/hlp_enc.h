#ifndef HLP_ENC_H
#define HLP_ENC_H

#include "libDos9.h"

#ifdef WIN32

    #include "iconv.h"

#else

    #include <iconv.h>

#endif

int Hlp_StringConvert(iconv_t cd, ESTR* lpEsReturn, char* lpToken);

#endif // HLP_ENC_H
