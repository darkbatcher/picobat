/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
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
#ifndef PBAT_VERSION_INFO_H
#define PBAT_VERSION_INFO_H

#include "../../config.h"

#ifdef PACKAGE_VERSION
#define PBAT_VERSION PACKAGE_VERSION
#else
#define PBAT_VERSION "217.1"
#endif

#ifdef PACKAGE_BUGREPORT
#define PBAT_BUGREPORT PACKAGE_BUGREPORT
#else
#define PBAT_BUGREPORT "darkbatcher@picobat.org"
#endif

#ifdef PACKAGE_URL
#define PBAT_URL PACKAGE_URL
#else
#define PBAT_URL "http://www.picobat.org/"
#endif

#ifdef HOST
#define PBAT_HOST HOST
#else
#define PBAT_HOST "Unknown"
#endif


#define PBAT_AUTHORS "Romain Garbi, Teddy Astie"
#define PBAT_BUILDDATE __DATE__
#define PBAT_BUILDYEAR "2018"
#define PBAT_BUILDCOMMENTS "Beta release"

/* Choose between different known operating systems.
   There is obviously more operating systems, but
   there are those that we support */
#if defined(WIN32)
#define PBAT_OS "WINDOWS"
#elif defined(__linux__)
#define PBAT_OS "GNU/Linux"
#elif defined(__NetBSD__)
#define PBAT_OS "NetBSD"
#elif defined(__DragonFly__)
#define PBAT_OS "DragonFlyBSD"
#elif defined(__OpenBSD__)
#define PBAT_OS "OpenBSD"
#else
#define PBAT_OS PBAT_HOST /* Use build triplet as failback OS */
#endif

#if defined(WIN32)
#define PBAT_OS_TYPE "WINDOWS"
#elif !defined(WIN32)
#define PBAT_OS_TYPE "*NIX"
#else
#define PBAT_OS_TYPE "Unknown"
#endif

#endif // PBAT_VERSION_INFO_H
