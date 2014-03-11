#ifndef DOS9_FILELIB_H
#define DOS9_FILELIB_H

#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
    #include <windows.h>
#else
    #define FILE_SYM_LINK "/proc/self/exe"
#endif

#endif
