#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <math.h>
#include <float.h>
#include <fcntl.h>

#ifdef WIN32
#include <io.h>
#endif

#define XSTR(x) #x
#define STR(x) XSTR(x)

#ifndef DBL_DECIMAL_DIG
#define DBL_DECIMAL_DIG 17
#endif // DBL_DECIMAL_DIG

#ifndef FLT_DECIMAL_DIG
#define FLT_DECIMAL_DIG 9
#endif // DBL_DECIMAL_DIG

int main(void)
{
    char cmax = CHAR_MAX, cmin = CHAR_MIN;
    unsigned char ucmax = UCHAR_MAX, ucmin = 0;
    short smax = SHRT_MAX, smin = SHRT_MIN;
    unsigned short usmax = USHRT_MAX, usmin = 0;
    int imax = INT_MAX, imin = INT_MIN;
    unsigned int uimax = UINT_MAX, uimin = 0;
    long long llmax = LLONG_MAX, llmin = LLONG_MIN;
    unsigned long long ullmax = ULLONG_MAX, ullmin = 0;
    float fmax = FLT_MAX, fmin = FLT_MIN;
    double dmax = DBL_MAX, dmin = DBL_MIN;

    printf("#define DUMP_CHAR_MAX \"%d\"\n", cmax);
    printf("#define DUMP_CHAR_MIN \"%d\"\n", cmin);
    printf("#define DUMP_UCHAR_MAX \"%u\"\n", ucmax);
    printf("#define DUMP_UCHAR_MIN \"%u\"\n", ucmin);
    printf("#define DUMP_SHRT_MAX \"%d\"\n", smax);
    printf("#define DUMP_SHRT_MIN \"%d\"\n", smin);
    printf("#define DUMP_USHRT_MAX \"%u\"\n", usmax);
    printf("#define DUMP_USHRT_MIN \"%u\"\n", usmin);
    printf("#define DUMP_INT_MAX \"%d\"\n", imax);
    printf("#define DUMP_INT_MIN \"%d\"\n", imin);
    printf("#define DUMP_UINT_MAX \"%u\"\n", uimax);
    printf("#define DUMP_UINT_MIN \"%u\"\n", uimin);
    printf("#define DUMP_LLONG_MAX \"%lld\"\n", llmax);
    printf("#define DUMP_LLONG_MIN \"%lld\"\n", llmin);
    printf("#define DUMP_ULLONG_MAX \"%llu\"\n", ullmax);
    printf("#define DUMP_ULLONG_MIN \"%llu\"\n", ullmin);
    printf("#define DUMP_FLT_MAX \"%." STR(FLT_DECIMAL_DIG) "g\"\n", fmax);
    printf("#define DUMP_FLT_MIN \"%." STR(FLT_DECIMAL_DIG) "g\"\n", fmin);
    printf("#define DUMP_DBL_MAX \"%." STR(DBL_DECIMAL_DIG) "g\"\n", dmax);
    printf("#define DUMP_DBL_MIN \"%." STR(DBL_DECIMAL_DIG) "g\"\n", dmin);

    return 0;
}
