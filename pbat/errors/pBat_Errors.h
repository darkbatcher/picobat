/*
 *
 *   pBat - A Free, Cross-platform command prompt - The pBat project
 *   Copyright (C) 2010-2017 Romain GARBI
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

#ifndef PBAT_MESSAGES_H
#define PBAT_MESSAGES_H



#ifdef PBAT_DEBUG_MODE

#define DEBUG(Msg,...) fprintf(stderr, __FILE__ "\t" __func__ ":%d\n\t" Msg "\n", __LINE__, ##__VA_ARGS__)
#define DEBUG_(a) fprintf( stderr, "$ %s (line %d) * %d\n", __func__, __LINE__, a)
#define PBAT_DEBUG(a) fprintf( stderr, "$ %s (line %d) * %s\n", __func__, __LINE__, a)

#else

#define DEBUG(Msg, ...)
#define DEBUG_(a)
#define PBAT_DEBUG(a)

#endif

void pBat_ShowErrorMessage(unsigned int iErrorNumber, const char* lpComplement, int iExitCode);
void pBat_LoadErrors(void);

#define PBAT_NO_ERROR                        0
#define PBAT_FILE_ERROR                      1
#define PBAT_DIRECTORY_ERROR                 2
#define PBAT_COMMAND_ERROR                   3
#define PBAT_UNEXPECTED_ELEMENT              4
#define PBAT_BAD_COMMAND_LINE                5
#define PBAT_LABEL_ERROR                     6
#define PBAT_EXTENSION_DISABLED_ERROR        7
#define PBAT_EXPECTED_MORE                   8
#define PBAT_INCOMPATIBLE_ARGS               9
#define PBAT_UNABLE_RENAME                  10
#define PBAT_MATH_OUT_OF_RANGE              11
#define PBAT_MATH_DIVIDE_BY_0               12
#define PBAT_MKDIR_ERROR                    13
#define PBAT_RMDIR_ERROR                    14
#define PBAT_STREAM_MODULE_ERROR            15
#define PBAT_SPECIAL_VAR_NON_ASCII          16
#define PBAT_ARGUMENT_NOT_BLOCK             17
#define PBAT_FOR_BAD_TOKEN_SPECIFIER        18
#define PBAT_FOR_TOKEN_OVERFLOW             19
#define PBAT_FOR_USEBACKQ_VIOLATION         20
#define PBAT_FAILED_ALLOCATION              21
#define PBAT_CREATE_PIPE                    22
#define PBAT_FOR_LAUNCH_ERROR               23
#define PBAT_FOR_BAD_INPUT_SPECIFIER        24
#define PBAT_FOR_TRY_REASSIGN_VAR           25
#define PBAT_INVALID_EXPRESSION             26
#define PBAT_INVALID_TOP_BLOCK              27
#define PBAT_UNABLE_DUPLICATE_FD            28
#define PBAT_UNABLE_CREATE_PIPE             29
#define PBAT_UNABLE_SET_ENVIRONMENT         30
#define PBAT_INVALID_REDIRECTION            31
#define PBAT_ALREADY_REDIRECTED             32
#define PBAT_MALFORMED_BLOCKS               33
#define PBAT_NONCLOSED_BLOCKS               34
#define PBAT_UNABLE_ADD_COMMAND             35
#define PBAT_UNABLE_REMAP_COMMANDS          36
#define PBAT_TRY_REDEFINE_COMMAND           37
#define PBAT_UNABLE_REPLACE_COMMAND         38
#define PBAT_UNABLE_SET_OPTION		        39
#define PBAT_COMPARISON_FORBIDS_STRING      40
#define PBAT_FAILED_FORK                    41
#define PBAT_NO_MATCH                       42
#define PBAT_NO_VALID_FILE                  43
#define PBAT_INVALID_NUMBER                 44
#define PBAT_TOO_MANY_ARGS                  45
#define PBAT_INVALID_IF_EXPRESSION          46
#define PBAT_UNABLE_MOVE                    47
#define PBAT_UNABLE_COPY                    48
#define PBAT_UNABLE_DELETE                  49
#define PBAT_UNABLE_MKDIR                   50
#define PBAT_UNABLE_RMDIR                   51
#define PBAT_MOVE_NOT_RENAME                52
#define PBAT_INVALID_CODEPAGE               53
#define PBAT_BREAK_ERROR                    54
#define PBAT_INVALID_LABEL                  55
#define PBAT_LOCK_MUTEX_ERROR               56
#define PBAT_RELEASE_MUTEX_ERROR            57
#define PBAT_UNABLE_CAT                     58
#define PBAT_UNABLE_LOAD_MODULE             59
#define PBAT_INCOMPATIBLE_MODULE            60
#define PBAT_MODULE_LOADED                  61
#define PBAT_ERROR_MESSAGE_NUMBER           62

#define PBAT_PRINT_C_ERROR 0x80

extern const char* lpErrorMsg[PBAT_ERROR_MESSAGE_NUMBER];
extern const char* lpQuitMessage;

#endif
