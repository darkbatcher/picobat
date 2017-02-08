/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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
#ifndef DOS9_VERSION_INFO_H
#define DOS9_VERSION_INFO_H

#include "../../config.h"

#ifdef PACKAGE_VERSION
#define DOS9_VERSION PACKAGE_VERSION
#else
#define DOS9_VERSION "217.1"
#endif

#ifdef PACKAGE_BUGREPORT
#define DOS9_BUGREPORT PACKAGE_BUGREPORT
#else
#define DOS9_BUGREPORT "darkbatcher@dos9.org"
#endif

#ifdef PACKAGE_URL
#define DOS9_URL PACKAGE_URL
#else
#define DOS9_URL "http://www.dos9.org/"
#endif

#ifdef HOST
#define DOS9_HOST HOST
#else
#define DOS9_HOST "Unknown"
#endif


#define DOS9_AUTHORS "Romain Garbi, Teddy Astie"
#define DOS9_BUILDDATE __DATE__
#define DOS9_BUILDYEAR "2017"
#define DOS9_BUILDCOMMENTS "Beta release"

/* Choose between different known operating systems.
   There is obviously more operating systems, but
   there are those that we support */
#if defined(WIN32)
#define DOS9_OS "WINDOWS"
#elif defined(__linux__)
#define DOS9_OS "GNU/Linux"
#elif defined(__NetBSD__)
#define DOS9_OS "NetBSD"
#elif defined(__DragonFly__)
#define DOS9_OS "DragonFlyBSD"
#elif defined(__OpenBSD__)
#define DOS9_OS "OpenBSD"
#else
#define DOS9_OS DOS9_HOST /* Use build triplet as failback OS */
#endif

#if defined(WIN32)
#define DOS9_OS_TYPE "WINDOWS"
#elif !defined(WIN32)
#define DOS9_OS_TYPE "*NIX"
#else
#define DOS9_OS_TYPE "Unknown"
#endif

#endif // DOS9_VERSION_INFO_H
