/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
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

#ifndef DOS9_MESSAGES_H
#define DOS9_MESSAGES_H



#ifdef DOS9_DEBUG_MODE

#define DEBUG(Msg,...) fprintf(stderr, __FILE__ "\t" __func__ ":%d\n\t" Msg "\n", __LINE__, ##__VA_ARGS__)
#define DEBUG_(a) fprintf( stderr, "$ %s (line %d) * %d\n", __func__, __LINE__, a)
#define DOS9_DEBUG(a) fprintf( stderr, "$ %s (line %d) * %s\n", __func__, __LINE__, a)

#else

#define DEBUG(Msg, ...)
#define DEBUG_(a)
#define DOS9_DEBUG(a)

#endif

void Dos9_ShowErrorMessage(unsigned int iErrorNumber, const char* lpComplement, int iExitCode);
void Dos9_LoadErrors(void);

#define DOS9_FILE_ERROR                      0
#define DOS9_DIRECTORY_ERROR                 1
#define DOS9_COMMAND_ERROR                   2
#define DOS9_UNEXPECTED_ELEMENT              3
#define DOS9_BAD_COMMAND_LINE                4
#define DOS9_LABEL_ERROR                     5
#define DOS9_EXTENSION_DISABLED_ERROR        6
#define DOS9_EXPECTED_MORE                   7
#define DOS9_INCOMPATIBLE_ARGS               8
#define DOS9_UNABLE_RENAME                   9
#define DOS9_MATH_OUT_OF_RANGE              10
#define DOS9_MATH_DIVIDE_BY_0               11
#define DOS9_MKDIR_ERROR                    12
#define DOS9_RMDIR_ERROR                    13
#define DOS9_STREAM_MODULE_ERROR            14
#define DOS9_SPECIAL_VAR_NON_ASCII          15
#define DOS9_ARGUMENT_NOT_BLOCK             16
#define DOS9_FOR_BAD_TOKEN_SPECIFIER        17
#define DOS9_FOR_TOKEN_OVERFLOW             18
#define DOS9_FOR_USEBACKQ_VIOLATION         19
#define DOS9_FAILED_ALLOCATION              20
#define DOS9_CREATE_PIPE                    21
#define DOS9_FOR_LAUNCH_ERROR               22
#define DOS9_FOR_BAD_INPUT_SPECIFIER        23
#define DOS9_FOR_TRY_REASSIGN_VAR           24
#define DOS9_INVALID_EXPRESSION             25
#define DOS9_INVALID_TOP_BLOCK              26
#define DOS9_UNABLE_DUPLICATE_FD            27
#define DOS9_UNABLE_CREATE_PIPE             28
#define DOS9_UNABLE_SET_ENVIRONMENT         29
#define DOS9_INVALID_REDIRECTION            30
#define DOS9_ALREADY_REDIRECTED             31
#define DOS9_MALFORMED_BLOCKS               32
#define DOS9_NONCLOSED_BLOCKS               33
#define DOS9_UNABLE_ADD_COMMAND             34
#define DOS9_UNABLE_REMAP_COMMANDS          35
#define DOS9_TRY_REDEFINE_COMMAND           36
#define DOS9_UNABLE_REPLACE_COMMAND         37
#define DOS9_UNABLE_SET_OPTION		        38
#define DOS9_COMPARISON_FORBIDS_STRING      39
#define DOS9_FAILED_FORK                    40
#define DOS9_NO_MATCH                       41
#define DOS9_NO_VALID_FILE                  42
#define DOS9_INVALID_NUMBER                 43
#define DOS9_TOO_MANY_ARGS                  44
#define DOS9_INVALID_IF_EXPRESSION          45
#define DOS9_UNABLE_MOVE                    46
#define DOS9_UNABLE_COPY                    47
#define DOS9_UNABLE_DELETE                  48
#define DOS9_UNABLE_MKDIR                   49
#define DOS9_UNABLE_RMDIR                   50
#define DOS9_MOVE_NOT_RENAME                51
#define DOS9_INVALID_CODEPAGE               52
#define DOS9_BREAK_ERROR                    53
#define DOS9_INVALID_LABEL                  54
#define DOS9_ERROR_MESSAGE_NUMBER           55

#define DOS9_PRINT_C_ERROR 0x80

extern const char* lpErrorMsg[DOS9_ERROR_MESSAGE_NUMBER];
extern const char* lpQuitMessage;

#endif
