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

#ifndef DOS9_CORE_H_INCLUDED
#define DOS9_CORE_H_INCLUDED

#include <libDos9.h>
#include "../errors/Dos9_Errors.h"

#include "Dos9_Env.h"

#include "Dos9_Var.h"
#include "Dos9_Expand.h"

#include "Dos9_Args.h"
#include "Dos9_Jump.h"
#include "Dos9_VersionInfo.h"
#include "Dos9_ShowIntro.h"
#include "Dos9_FilePath.h"
#include "Dos9_Parse.h"
#include "Dos9_Read.h"
#include "Dos9_Stream.h"
#include "Dos9_Run.h"
#include "Dos9_Expand.h"
#include "Dos9_SplitPath.h"
#include "Dos9_ParseBlock.h"
#include "Dos9_ExpandAlias.h"
#include "Dos9_OsStream.h"

#include "Dos9_Globals.h"

#include "Dos9_ExitInt.h"

#ifdef WIN32

#include <conio.h>

#elif !defined(WIN32)

#define stricmp strcasecmp
#define strnicmp strncasecmp

#endif

#endif // DOS9_CORE_H_INCLUDED
