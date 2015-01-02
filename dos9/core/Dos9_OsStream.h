/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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
 */
#ifndef DOS9_OSSTREAM_H
#define DOS9_OSSTREAM_H

/* a macro to change names of the OS defined stream on a per OS
   basis */

#if defined(WIN32)
#define Dos9_GetOsStreamName(str) str
#elif defined(_POSIX_C_SOURCE)
#define Dos9_GetOsStreamName(str) _Dos9_GetOsStreamName(str)
const char* _Dos9_GetOsStreamName(const char* str);
#endif

#endif
