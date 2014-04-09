#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "Dos9_Debug.h"

void Dos9_ShowDebug(const char* format,...)
{
    va_list vaArgs;

    va_start(vaArgs, format);
    vfprintf(stderr, format, vaArgs);

    va_end(vaArgs);
}
