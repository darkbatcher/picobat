#ifndef HLP_ENC_H
#define HLP_ENC_H

#include <libDos9.h>
#include <iconv.h>

int Hlp_StringConvert(iconv_t cd, ESTR* lpEsReturn, char* lpToken);

#endif // HLP_ENC_H
