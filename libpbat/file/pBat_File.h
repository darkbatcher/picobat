/*
 *
 *   libpBat - The pBat project
 *   Copyright (C) 2010-2016 Romain GARBI
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
#ifndef PBAT_FILELIB_H
#define PBAT_FILELIB_H

#include <dirent.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* Define the values of symlinks on process information (especially, the path
   of the current executable) via procfs. This is a big concern however,
   because there's no standards used on the procfs system, so that
   every OS have different syntax. On some other operating systems (MacOS X,
   FreeBSD, SunOS, Solaris, AIX), platform dependent code sould be written in
   order to make the file portable.

	OSes Currently suppported :

	* MS-Windows (NT)
	* GNU/Linux
	* NetBSD
	* DragonFlyBSD
	* OpenBSD

 */

#if defined __linux__

    #define PBAT_FILE_SYM_LINK "/proc/self/exe"

#elif defined __NetBSD__

    #define PBAT_FILE_SYM_LINK "/proc/curproc/exe"

#elif ( defined __DragonFly__ ) || ( defined __OpenBSD__)

    #define PBAT_FILE_SYM_LINK "/proc/curproc/file"

#elif ( defined WIN32 )

    #include <windows.h>

#endif

#include "../libpBat.h"
#include "../libpBat-int.h"

#endif
