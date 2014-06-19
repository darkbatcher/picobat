#ifndef DOS9_FILELIB_H
#define DOS9_FILELIB_H

#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* Define the values of symlinks on process information (especially, the path
   of the current executable) via procfs. This is a big concern however,
   because there's no standards used on the procfs system, so that
   every OS have differebt syntax
 */

#if defined __linux__

    #define DOS9_FILE_SYM_LINK "/proc/self/exe"

#elif defined __NetBSD__

    #define DOS9_FILE_SYM_LINK "/proc/curproc/exe"

#elif ( defined __DragonFly__ ) || ( defined __OpenBSD__)

    #define DOS9_FILE_SYM_LINK "/proc/curproc/file"

#elif ( defined WIN32 )

    #include <windows.h>

#endif

#include "../libDos9-int.h"

#endif
