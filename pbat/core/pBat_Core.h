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

#ifndef PBAT_CORE_H_INCLUDED
#define PBAT_CORE_H_INCLUDED

#include <libpBat.h>
#include "../errors/pBat_Errors.h"

#include "pBat_Env.h"

#include "pBat_Var.h"
#include "pBat_Expand.h"

#include "pBat_Args.h"
#include "pBat_DirStack.h"
#include "pBat_Jump.h"
#include "pBat_VersionInfo.h"
#include "pBat_ShowIntro.h"
#include "pBat_FilePath.h"
#include "pBat_Parse.h"
#include "pBat_Read.h"
#include "pBat_Stream.h"
#include "pBat_Run.h"
#include "pBat_Expand.h"
#include "pBat_SplitPath.h"
#include "pBat_ParseBlock.h"
#include "pBat_ExpandDef.h"
#include "pBat_OsStream.h"
#include "pBat_FdInheritance.h"
#include "pBat_ReadScript.h"
#include "pBat_Clone.h"
#include "pBat_EnvStack.h"
#include "pBat_Globals.h"
#include "pBat_ExitInt.h"
#include "pBat_Exec.h"
#include "pBat_Prompt.h"
#include "pBat_Completion.h"

#endif // PBAT_CORE_H_INCLUDED
