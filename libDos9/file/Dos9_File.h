#ifndef DOS9_FILELIB_H
#define DOS9_FILELIB_H

#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#ifdef WIN32
#include <windows.h>

/* define usefull symlinks on process information */

#elif defined __linux

    #define DOS9_FILE_SYM_LINK "/proc/self/exe"

#elif defined __NetBSD__

    #define DOS9_FILE_SYM_LINK "/proc/curproc/exe"

#elif ( defined __DragonFly__ ) || ( defined __OpenBSD__)

    #define DOS9_FILE_SYM_LINK "/proc/curproc/file"

#endif

#endif
