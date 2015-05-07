/*
 *
 *   libDos9 - The Dos9 project
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
 *
 */

#ifndef LIBDOS9_INCLUDED
#define LIBDOS9_INCLUDED

/* We don't define _POSIX_C_SOURCE value here, just set it by the -D compiler
   flag, to the value you want. I don't actually now for wich revision of
   POSIX i'm conforming, although AFAIK I only use POSIX functions for *nix
   oses. If you try to build it in non POSIX conforming systems, (by default)
   you should probably append the given argurments to the CFLAGS variables
   when configuring:

	-D_POSIX_C_SOURCE=200809L

   I have not tried with previous revisions, but this it seems to work on
   NetBSD 6.1
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <dirent.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>




#ifdef WIN32



#include <windows.h>
#include <process.h>
#include <io.h>

#define stat _stat


/** \defgroup FILE_ATTRIBUTES File attributes constants
     \{ */


/** The file is an archive */
#define DOS9_FILE_ARCHIVE FILE_ATTRIBUTE_ARCHIVE



/** The file compressed */
#define DOS9_FILE_COMPRESSED FILE_ATTRIBUTE_COMPRESSED

/** The file is hidden */
#define DOS9_FILE_HIDDEN FILE_ATTRIBUTE_HIDDEN

/** The file has offline attribute */
#define DOS9_FILE_OFFLINE FILE_ATTRIBUTE_OFFLINE



/** The file is read-only */
#define DOS9_FILE_READONLY FILE_ATTRIBUTE_READONLY

/** The file has system attribute */
#define DOS9_FILE_SYSTEM FILE_ATTRIBUTE_SYSTEM

/** The file is a directory */
#define DOS9_FILE_DIR FILE_ATTRIBUTE_DIRECTORY

/** \} */

#define flushall() _flushall()
#define dup(a) _dup(a)
#define dup2(a,b) _dup2(a,b)
#define write(a,b,c) _write(a,b,c)
#define read(a,b,c) _read(a,b,c)
#define access(a,b) _acess(a,b)
#define O_WRONLY _O_WRONLY
#define O_RDONLY _O_RDONLY
#define O_TRUNC _O_TRUNC
#define O_TEXT _O_TEXT



#define S_IREAD _S_IREAD
#define O_IWRITE _S_IWRITE
#define Dos9_GetFileAttributes(lpcstr) GetFileAttributes(lpcstr)

#define _Dos9_Pipe(descriptors, size, mode) _pipe(descriptors, size, mode)

#else

#include <unistd.h>
#include <pthread.h>
#include <stdarg.h>
#include <sys/stat.h>

#define stricmp(a,b) strcasecmp(a,b)
#define strnicmp(a,b,c) strncasecmp(a,b,c)

#define DOS9_FILE_DIR S_IFDIR
#define DOS9_FILE_EXECUTE S_IXUSR
#define DOS9_FILE_READ S_IRUSR
#define DOS9_FILE_WRITE S_IWUSR
#define DOS9_FILE_REGULAR S_IFREG
#define DOS9_FILE_ARCHIVE 0
#define DOS9_FILE_COMPRESSED 0
#define DOS9_FILE_HIDDEN 0
#define DOS9_FILE_OFFLINE 0
#define DOS9_FILE_READONLY 0
#define DOS9_FILE_SYSTEM 0

#define Dos9_GetFileAttributes(lpName)
#define _Dos9_Pipe(descriptors, size, mode) pipe(descriptors)

#endif

#ifdef _POSIX_C_SOURCE

typedef pthread_t       THREAD;
typedef pthread_mutex_t MUTEX;
typedef pid_t           PROCESS;


#elif defined WIN32

typedef DWORD  THREAD;
typedef HANDLE MUTEX;
typedef int    PROCESS;

#endif


#define LIBDOS9

#define TRUE 1
#define FALSE 0


LIBDOS9 int Dos9_LibInit(void);
LIBDOS9 void Dos9_LibClose(void);
void _Dos9_Estr_Close(void);
int _Dos9_Estr_Init(void);
int _Dos9_Thread_Init(void);
void _Dos9_Thread_Close(void);

/** \defgroup THREAD_REF Thread reference
    \{ */

/** \defgroup THREAD_STRUCT Thread structures
    \{ */

/** \typedef THREAD
    An type that represent HTHREAD on window and pthread_t on GNU/Linux */

/** \} */

/** \defgroup TREAD_FUNC Thread function
    \{ */

/** \fn LIBDOS9 THREAD Dos9_BeginThread(void(*lpFunction)(void*) , int iMemAmount, void* lpArgList)
    Start a thread.
    \param lpFunction : A pointer to a function where the thread will start.
    \param iMemAmount : The amout of memory to reserve for the thread. If 0, the amount is automatically adjusted
    \param lpArgList : The only argument of lpFunction
    \return Returns a handle to the process in a THREAD structure.

    \fn LIBDOS9 void Dos9_EndThread(void)
    Ends current thread

    \fn LIBDOS9 void Dos9_EndThreadEx(void* lpExitCode)
    Ends the current thread with an exit code
    \param lpExitCode : The thread exit code

    \fn LIBDOS9 void Dos9_WaitForThread(THREAD hThread)
    Waits until the specified thread terminates
    \param hThread : A handle to the thread

    \fn LIBDOS9 void Dos9_GetThreadExitCode(THREAD hThread, void* lpResult)
    Get the process exit code.
    \param hThread : A handle to the process
    \param lpResult : A pointer to a location where the result will be stored

\} */
/** \} */

/** \defgroup STACK_REF Stack references
    \{ */
/** \defgroup STACK_STRUCT Stack structures
    \{ */
/** A structure designed to be used as stack in many diferent uses */
typedef struct STACK {
	int iFlag; /**< An int that stores a programmer-defined constant. Usefull for determining of wich type the element is for example. */
	void* ptrContent; /**< A pointer to the element */
	struct STACK* lpcsPrevious; /**< A pointer to the previous element */
} STACK,*LPSTACK;

/** \} */
/** \defgroup STACK_FUNCTIONS Stack functions
    \{ */

/** Push an element on the top of a stack
    \param  lpcsStack : The stack onto wich the element will be pushed (can be NULL, then a new stack will be alocated)
    \param  ptrContent : A pointer to the element
    \param  iFlag : A programmer-defined flag
    \return Returns a pointer to the new top-element, NULL if the function failled
    \attention Pushing an element does not make copies of element, it just stores a pointer into the stack
*/
LIBDOS9     LPSTACK Dos9_PushStack(LPSTACK lpcsStack, void* ptrContent);

/** Pop an element from the top of a stack
    \param lpcsStack : The stack to be poped
    \return Returns a pointer to the new top-element, NULL if the function failled
    \attention Poping an element does not free it, it just removes it from the stack
*/
LIBDOS9     LPSTACK Dos9_PopStack(LPSTACK lpcsStack, void(*pFunc)(void*));

/** \brief Get the element on top of a stack
    \param lpcsStack : The stack from which the function will return the top element
    \param ptrContent : A pointer to a pointer that will recieve the element
    \return Returns 0 if the function succeded, 1 else.
*/
LIBDOS9     int Dos9_GetStack(LPSTACK lpcsStack, void** ptrContent);

/** \brief Clears a stack
    \param lpcsStack : The stack the function will clear
    \return Returns 0 if the function succeded, 1 else.
    \attention Clearing a stack does not free all it element, it just destroys the stack
    \deprecated This function is deprecated, use Dos9_ClearStack() instead */
LIBDOS9     int Dos9_ClearStack(LPSTACK lpcsStack, void(*pFunc)(void*));

/** \} */

/** \defgroup STACK_MACROS Stack macros
    \} */

/** Provided for compatibility purposes */
#define Dos9_PushCallStack Dos9_PushStack

/** Provided for compatibility purposes */
#define Dos9_PopCallStack Dos9_PopStack

/** Provided for compatibility purposes */
#define Dos9_GetCallStack Dos9_GetStack

/** Provided for compatibility purposes */
#define Dos9_GetClearCallStack Dos9_ClearStack

/** \} */

/** \} */

/** \defgroup ESTR_REF  Extended strings references
    \{ */

/** \defgroup ESTR_STRUCT Extended strings structures
    \{ */

/** A structure representing an extended string (e.g. a string that don't have a specific length) */
typedef struct ESTR {
	char* ptrString;/**< A pointer to a NULL terminated string that contains the string. Don't access it directly, use \link ESTR_MACRO extended strings macros\endlink to get its content*/
	int iLenght;/**< An integer that stores the length of the buffer pointed by ptrString */
} ESTR, *LPESTR;

/** \} */

/** \defgroup ESTR_MACROS Extended strings macros
    \{
*/

/** Retrieves a null-terminated string from an ESTR structure
    \deprecated This function is deprecated and remain here only for compatibility purpose. You should better use Dos9_EsToChar() macro */
#define EsToChar(a) a->ptrString

/** Retrieves a null-terminated string from an ESTR structure */
#define Dos9_EsToChar(a) a->ptrString

/** the newlines will be converted to '\r\n' */
#define DOS9_NEWLINE_WINDOWS 0

/** the newlines will be converted to '\n' */
#define DOS9_NEWLINE_UNIX 1
#define DOS9_NEWLINE_LINUX DOS9_NEWLINE_UNIX
/* defined only for compatibility purpose */


/** the newlines will be converted to '\r' */
#define DOS9_NEWLINE_MAC 2

/** Sets the newline mode that changes behavior of Dos9_EsGet() */
#define Dos9_SetNewLineMode(type) _Dos9_NewLine=type

/** \} */

/** \defgroup ESTR_GLOBAL Extended string global vars
    \{ */

/** A value that store the new line setting of LibDos9
    \attention Modifying it directly is not safe, use Dos9_SetNewLineMode() to set the new-line mode */
extern int _Dos9_NewLine;

/** \} */

/** \defgroup ESTR_FUNC Extended strings functions
    \{ */

/** \brief Initializes and allocate an extended string
    \return Return a pointer to an ESTR structure, or NULL if the function fails */
LIBDOS9 ESTR*           Dos9_EsInit(void);

/** \brief Get extended string from a file
    The function reads in a file until it encounters a line-feed or a carriage return, and stores it in an extended string
    \param ptrESTR : A pointer to an extended string that will recieve the content read
    \param ptrFile : A standard pointer in which the function will read
    \return Returns 0 if the end of file has not been reached, 1 otherwise.
    \note The behavior of the function is modified by the value of _Dos9_NewLine */
LIBDOS9 int             Dos9_EsGet(ESTR* ptrESTR, FILE* ptrFile);

/** \brief Copy a NULL-terminated string into an extended string
    \param ptrESTR : A pointer to an extended string that will recieve the copy of the string
    \param ptrChaine : A pointer to a NULL terminated string that will be copied
    \return Returns 0 if the function succeded, or 1 otherwise */
LIBDOS9 int             Dos9_EsCpy(ESTR* ptrESTR, const char* ptrChaine);

/** \brief Copy at most N bytes from a NULL-terminated string into an extended string
    \param ptrESTR : A pointer to an extended string that will recieve the copy of the string
    \param ptrChaine : A pointer to a NULL terminated string that will be copied
    \param iSize : The maximum number of bytes to be cat
    \return Returns 0 if the function succeded, or 1 otherwise */
LIBDOS9 int             Dos9_EsCpyN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize);

/** \brief Cat a NULL-terminated string and an extended string
    \param ptrESTR : A pointer to an extended string that will be cat
    \param ptrChaine : A pointer to a NULL terminated string to be cat
    \return Returns 0 if the function succeded, or 1 otherwise */
LIBDOS9 int             Dos9_EsCat(ESTR* ptrESTR, const char* ptrChaine);

/** \brief Cat at most N bytes from a NULL-terminated string to an extended string
    \param ptrESTR : A pointer to an extended string that will be cat
    \param ptrChaine : A pointer to a NULL terminated string to be cat
    \param iSize : The maximun number of bytes to be cat
    \return Returns 0 if the function succeded, or 1 otherwise */
LIBDOS9 int             Dos9_EsCatN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize);

/** \brief Frees an extended string
    \param ptrESTR: A pointer to an extended string to be freed
    \return Returns 0 if the function succeded, or 1 otherwise */
LIBDOS9 int             Dos9_EsFree(ESTR* ptrESTR);

/** \brief Copy an extended string into an extended string
    \param ptrSource : A pointer to an extended string that will recieve the copy of the string
    \param ptrDest : A pointer to an extended string that will be copied
    \return Returns 0 if the function succeded, or 1 otherwise */
LIBDOS9 int             Dos9_EsCpyE(ESTR* ptrSource, const ESTR* ptrDest);


/** \brief Cat an extended string and an extended string
    \param ptrDest : A pointer to an extended string that will be cat
    \param ptrSource : A pointer to an extended string to be cat
    \return Returns 0 if the function succeded, or 1 otherwise */
LIBDOS9 int             Dos9_EsCatE(ESTR* ptrDest, const ESTR* ptrSource);

/** \brief Replace a string occurence in an extended string
    \param ptrESTR : A pointer to an extended string were occurences will be replaced
    \param ptrPatern : A pointer to a partern to replace
    \param ptrReplace : A pointer to a string that will replace occurences of pattern
    \return Returns 0 if the function succeded, or 1 otherwise.

*/
LIBDOS9 int             Dos9_EsReplace(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace);

/** \brief Replace at most N string occurence in an extended string
    \param ptrESTR : A pointer to an extended string were occurences will be replaced
    \param ptrPatern : A pointer to a partern to replace
    \param ptrReplace : A pointer to a string that will replace occurences of pattern
    \param iN : The maximum occurences number to be replaced
    \return Returns 0 if the function succeded, or 1 otherwise.

*/
LIBDOS9 int             Dos9_EsReplaceN(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace, int iN);


/** \} */
/** \} */


#ifndef DEFAULT_ESTR
#define DEFAULT_ESTR 32
#endif

size_t          _Dos9_EsTotalLen2(const char* ptrChaine, const char* ptrString);
size_t          _Dos9_EsTotalLen(const char* ptrChaine);
size_t          _Dos9_EsTotalLen3(const char* ptrChaine, size_t iSize);
size_t          _Dos9_EsTotalLen4(const size_t iSize);


/** \defgroup CMD_REF Command search reference
    \{ */
/** \defgroup CMD_STRUCT Command search structures
    \{ */

/** A custom-type that allows programmer to use flags with commands
    \note The flag -1 (0xffffffff) is reserved as an invalid COMMANDFLAG to determine if functions failed*/
typedef int COMMANDFLAG;

/** A structures that stores mapped command information */
typedef struct COMMANDLIST {
	void* lpCommandProc; /**< A pointer to the function associated to the command (must use cdelc) */
	int iLenght;  /**< An integer that stores the command name length */
	char* ptrCommandName; /**<A pointer to a NULL-terminated string that stores the command name */
	COMMANDFLAG cfFlag; /**< A COMMANDFLAG that has been choosed by programmer */
	struct COMMANDLIST* lpclLeftRoot; /**<A pointer to the left-branch COMMANDLIST structure */
	struct COMMANDLIST* lpclRightRoot;/**<A pointer to the right-branch COMMANDLIST structure */
} COMMANDLIST, *LPCOMMANDLIST;


/** A structure used to store information about commands before mapping it */
typedef struct COMMANDINFO {
	char* ptrCommandName; /**< A pointer to a null-terminated string that stores the command name */
	void* lpCommandProc; /**< A pointer to the function associated to the command (must use cdecl) */
	COMMANDFLAG cfFlag; /**< A programmer-programmer defined constant for determine the command type */
} COMMANDINFO,*LPCOMMANDINFO;

/** \} */
/** \defgroup CMD_FUNC Commade search function
    \{ */

/** \brief Maps COMMANDINFO structure into a COMMANDLIST structure.
    Maps COMMANDINFO structure into a COMMANDLIST structure. This allow optimization or searching time. Indeed the commands are stored sorted in a COMMANDLIST structures to minimize search time
    \param lpciCommandInfo : A pointer to an array of COMMANDLIST that stores information about commands to be mapped
    \param i : An int that indicates the number of COMMANDLIST elements in the array
    \return Returns a pointer to a COMMANDLIST structure. If the function fails, it returns NULL
    \note The parameter array passed throug lpciCommandInfo might no be remain unchanged. And if it didn't the function would modify nothing but the order of the elements in the array */
LIBDOS9 LPCOMMANDLIST   Dos9_MapCommandInfo(LPCOMMANDINFO lpciCommandInfo, int i);

LIBDOS9 LPCOMMANDLIST   Dos9_ReMapCommandInfo(LPCOMMANDLIST lpclCommandList);

/** \brief Adds dynamicly a command to a COMMANDLIST structure
    Add dynamicly a command to a COMMANDLIST structure. Note that the function does not re-arrrange COMMANDLIST structure to increase searching.Thus this function is not designed for intensive use. For intensive use, use Dos9_MapCommandInfo() instead
    \param lpciCommandInfo : A pointer to a COMMANDINFO structure that stores information about command to be added
    \param lpclListEntry : A pointer to a COMMANDLIST structure to which the command will be added
    \return Returns 0 if the functions succeed, 1 else.
    \note This function may cause performance wasting because it does not re-arrange the COMMANDLIST structure for optimizing search time.
*/
LIBDOS9 int             Dos9_AddCommandDynamic(LPCOMMANDINFO lpciCommandInfo, LPCOMMANDLIST* lpclListEntry);

/** \brief Frees a COMMANDLIST structure
    \param lpclList : A pointer to the COMMANDLIST structure to be freed
    \return Returns 0 if the functions succeed, 1 else.*/
LIBDOS9 int             Dos9_FreeCommandList(LPCOMMANDLIST lpclList);

/** \brief Search a command in a COMMANDLIST structure
    \param lpCommandLine : A pointer to a NULL-terminated string that begins with a command name
    \param lpclCommandList : A pointer to a COMMANDLIST structure in which the command will be searched
    \param lpcpCommandProcedure : A pointer to a pointer that will recieve the command-associated function.
    \return Return the COMMANDFLAG associated to the command, or returns -1 (0xffffffff) if no command has been found
	\note Before Dos9 version 2014.0.9, the function used to return the first match found if they were colisions. Since, it returns the largest
    */
LIBDOS9 COMMANDFLAG     Dos9_GetCommandProc(char* lpCommandLine, LPCOMMANDLIST lpclCommandList,void** lpcpCommandProcedure);

LIBDOS9 int				Dos9_ReplaceCommand(LPCOMMANDINFO lpciCommand, LPCOMMANDLIST lpclCommandList);
/** \} */
/** \} */

#define DOS9_ALIAS_FLAG 0x80000000

int                     _Dos9_FillCommandList(LPCOMMANDLIST lpclList, LPCOMMANDINFO lpciCommandInfo);
int                     _Dos9_PutSeed(LPCOMMANDINFO lpciCommandInfo, int iSegBottom, int iSegTop, LPCOMMANDLIST* lpclList);
int                     _Dos9_Sort(const void* ptrS, const void* ptrD);
size_t                  _Dos9_CountListItems(LPCOMMANDLIST lpclList);
LPCOMMANDINFO           _Dos9_FillInfoFromList(LPCOMMANDINFO lpCommandInfo, LPCOMMANDLIST lpCommandList);


/* *******************************************************************************
   *                            Console Functions                                *
   *******************************************************************************/

#define DOS9_CURSOR_SHOW 1
#define DOS9_CURSOR_HIDE 1

#if defined WIN32
#define DOS9_FOREGROUND_RED FOREGROUND_RED
#define DOS9_FOREGROUND_BLUE FOREGROUND_BLUE
#define DOS9_FOREGROUND_GREEN FOREGROUND_GREEN
#define DOS9_BACKGROUND_GREEN BACKGROUND_GREEN
#define DOS9_BACKGROUND_BLUE BACKGROUND_BLUE
#define DOS9_BACKGROUND_RED BACKGROUND_RED

#define DOS9_BACKGROUND_INT BACKGROUND_INTENSITY
#define DOS9_FOREGROUND_INT FOREGROUND_INTENSITY

#define DOS9_BACKGROUND_DEFAULT 0
#define DOS9_FOREGROUND_DEFAULT DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_BLUE

#define DOS9_GET_FOREGROUND(a) (a & 0x0F)
#define DOS9_GET_BACKGROUND(a) (a & 0xF0)
#define DOS9_GET_BACKGROUND_(a) ((a & 0xF0)>>4)

#else


#define DOS9_FOREGROUND_RED 0x01
#define DOS9_FOREGROUND_BLUE 0x04
#define DOS9_FOREGROUND_GREEN 0x02
#define DOS9_BACKGROUND_GREEN 0x20
#define DOS9_BACKGROUND_BLUE 0x40
#define DOS9_BACKGROUND_RED 0x10

#define DOS9_BACKGROUND_INT 0
#define DOS9_FOREGROUND_INT 0x08

#define DOS9_BACKGROUND_DEFAULT 0x0100
#define DOS9_FOREGROUND_DEFAULT 0x0200

#define DOS9_GET_FOREGROUND(a) (a & 0x20F)
#define DOS9_GET_FOREGROUND_(a) (a & 0x07)
#define DOS9_GET_BACKGROUND(a) (a & 0x1F0)
#define DOS9_GET_BACKGROUND_(a) ((a & 0xF0)>>4)


#endif

/** \defgroup CONSOLE_REF Console references
    \{ */

/** \defgroup CONSOLE_MACRO Console macros
    \{ */

/** \defgroup CONSOLE_COLOR Console color attributes
    \{ */

/** \def DOS9_BACKGROUND_DEFAULT
    The default background color */

/** \def DOS9_FOREGROUND_DEFAULT
    The default foreground color */

/** \def DOS9_FOREGROUND_INT
    The foreground color is more intense */

/** \def DOS9_BACKGROUND_INT
    The background color is more intense */

/** \def DOS9_FOREGROUND_BLUE
    The foreground color is blue */

/** \def DOS9_FOREGROUND_RED
    The foreground color is red */

/** \def DOS9_FOREGROUND_GREEN
    The foreground color is green */

/** \def DOS9_BACKGROUND_BLUE
    The background color is blue */

/** \def DOS9_BACKGROUND_RED
    The background color is red */

/** \def DOS9_BACKGROUND_GREEN
    The background color is green */

/** The background color is intense blue */
#define DOS9_BACKGROUND_IBLUE DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_INT

/** The background color is intense red */
#define DOS9_BACKGROUND_IRED DOS9_BACKGROUND_RED | DOS9_BACKGROUND_INT

/** The background color is intense green */
#define DOS9_BACKGROUND_IGREEN DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT

/** The background color is yellow */
#define DOS9_BACKGROUND_YELLOW DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN

/** The background color is cyan */
#define DOS9_BACKGROUND_CYAN DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_GREEN

/** The background color is magenta */
#define DOS9_BACKGROUND_MAGENTA DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED

/** The background color is white */
#define DOS9_BACKGROUND_WHITE DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN

/** The background color is black */
#define DOS9_BACKGROUND_BLACK 0

/** The background color is intense yellow */
#define DOS9_BACKGROUND_IYELLOW DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT

/** The background color is intense cyan */
#define DOS9_BACKGROUND_ICYAN DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT

/** The background color is intense magenta */
#define DOS9_BACKGROUND_IMAGENTA DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED | DOS9_BACKGROUND_INT

/** The background color is intense white */
#define DOS9_BACKGROUND_IWHITE DOS9_BACKGROUND_BLUE | DOS9_BACKGROUND_RED | DOS9_BACKGROUND_GREEN | DOS9_BACKGROUND_INT

/** The background color is intense black */
#define DOS9_BACKGROUND_IBLACK DOS9_BACKGROUND_INT

/** The foreground color is yellow */
#define DOS9_FOREGROUND_YELLOW DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN

/** The foreground color is cyan */
#define DOS9_FOREGROUND_CYAN DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_GREEN

/** The foreground color is magenta */
#define DOS9_FOREGROUND_MAGENTA DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED

/** The foreground color is white */
#define DOS9_FOREGROUND_WHITE DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN

/** The foreground color is black */
#define DOS9_FOREGROUND_BLACK 0

/** The foreground color is intense blue */
#define DOS9_FOREGROUND_IBLUE DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_INT

/** The foreground color is intense red */
#define DOS9_FOREGROUND_IRED DOS9_FOREGROUND_RED | DOS9_FOREGROUND_INT

/** The foreground color is intense green */
#define DOS9_FOREGROUND_IGREEN DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT

/** The foreground color is intense yellow */
#define DOS9_FOREGROUND_IYELLOW DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT

/** The foreground color is intense cyan */
#define DOS9_FOREGROUND_ICYAN DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT

/** The foreground color is intense magenta */
#define DOS9_FOREGROUND_IMAGENTA DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_INT

/** The foreground color is intense white */
#define DOS9_FOREGROUND_IWHITE DOS9_FOREGROUND_BLUE | DOS9_FOREGROUND_RED | DOS9_FOREGROUND_GREEN | DOS9_FOREGROUND_INT

/** The foreground color is intense black */
#define DOS9_FOREGROUND_IBLACK DOS9_FOREGROUND_INT

/** The default console color */
#define DOS9_COLOR_DEFAULT DOS9_BACKGROUND_DEFAULT | DOS9_FOREGROUND_DEFAULT

/** \} */

/** \def DOS9_GET_BACKGROUND(a)
    Gets the background color from a */

/** \def DOS9_GET_FOREGROUND(a)
    Gets the foreground color from a */

/** \} */

/** \defgroup CONSOLE_STRUCT Console structures
    \{ */

/** A type to store colors */
typedef int COLOR;


#ifndef WIN32
/** A structure to store console coordinates */
typedef struct CONSOLECOORD {
	short X; /**< The x coordinate (column number). Starts at 0 */
	short Y; /**< The y coordinate (line number). Starts at 0 */
} CONSOLECOORD, *LPCONSOLECOORD;
#else

typedef COORD CONSOLECOORD;

#endif

/**  \} */

/** \defgroup CONSOLE_FUNC Console functions
    \{ */

/** Clears the console screen */
LIBDOS9 void Dos9_ClearConsoleScreen(void);

/** Sets the whole console color
    \param iColor : The color to be set. It can be a combination of \link CONSOLE_COLOR console color attributes \endlink .
    \note Under GNU/Linux, the function also clears the screen
*/
LIBDOS9 void Dos9_SetConsoleColor(COLOR cColor);

/** Sets the console color
    \param iColor : The color to be set. It can be a combination of \link CONSOLE_COLOR console color attributes \endlink .
*/
LIBDOS9 void Dos9_SetConsoleTextColor(COLOR cColor);

/** Sets console cursor position
    \param iCoord : A CONSOLECOORD structures that contains the new cursor coordinates */
LIBDOS9 void Dos9_SetConsoleCursorPosition(CONSOLECOORD iCoord);

/** Retreives the console cursor position
    \return Returns a CONSOLECOORD structure that contains the cursor coordinates */
LIBDOS9 CONSOLECOORD Dos9_GetConsoleCursorPosition(void);

/** Changes the cursor state
    \param bVisible : specify wether the cursor should be visible (1) or hidden (0)
    \param iSize : The size of the cursor, between 0 and 100
*/
LIBDOS9 void Dos9_SetConsoleCursorState(int bVisible, int iSize);

/** Sets the console title
    \param lpTitle : a pointer to a string to be set as title
*/
LIBDOS9 void Dos9_SetConsoleTitle(char* lpTitle); /* change the console title */

/** \} */
/** \} */

/* *******************************************************************************
   *                         File searching functions                            *
   ******************************************************************************* */


/** \defgroup FILEMATCH_REF File matching references
    \{ */
/** \defgroup FILEMATCH_MACRO File matching macros
    \{ */

/** \defgroup FILEINFO_MACRO File Information macros
    \{ */

/** Retrieves last access time from a LPFILELIST structure. */
#define Dos9_GetAccessTime(lpList) lpList->stFileStats.st_atime
/** Retrieves creating time from a LPFILELIST structure. */
#define Dos9_GetCreateTime(lpList) lpList->stFileStats.st_ctime
/** Retrieves last modification time from a LPFILELIST structure. */
#define Dos9_GetModifTime(lpList) lpList->stFileStats.st_mtime
/** Retrieves size of file (in bytes) from a LPFILELIST structure. */
#define Dos9_GetFileSize(lpList) lpList->stFileStats.st_size
/** Retrieves the file attributes for a LPFILELIST structure. For more information on mode see \link FILE_ATTRIBUTES File attributes constants */
#define Dos9_GetFileMode(lpList) lpList->stFileStats.st_mode

/** \} */
/** \defgroup FILE_ATTRIBUTES File attributes constants
    \{ */

/** \def DOS9_FILE_ARCHIVE
    The file is an archive */

/** \def DOS9_FILE_COMPRESSED
    The file compressed */

/** \def DOS9_FILE_HIDDEN
    The file is hidden */

/** \def DOS9_FILE_OFFLINE
    The file has offline attribute */

/** \def DOS9_FILE_READONLY
    The file is read-only */

/** \def DOS9_FILE_SYSTEM
    The file has system attribute */

/** \def DOS9_FILE_DIR
    The file is a directory */

/** \} */
/** \defgroup MATCH_FLAG Match flags constants
    \{ */

/** Searches for mathching file only in directory that match with the regular expression */
#define DOS9_SEARCH_DEFAULT 0x00

/** Searches recursively the files matching in sub directories.
\attention If this flag is specified, only two part of the regular expression are considered, the static begin and the file name */
#define DOS9_SEARCH_RECURSIVE 0x01

/** Searches and returns only the first file */
#define DOS9_SEARCH_GET_FIRST_MATCH 0x02

/** Search does not return information about file */
#define DOS9_SEARCH_NO_STAT 0x04

/** Search does not return the '..' and '.' directories */
#define DOS9_SEARCH_NO_PSEUDO_DIR 0x08

/** Search complies with traditional dir behaviour */
#define DOS9_SEARCH_DIR_MODE 0x10

/** \} */
/** \} */
/** \defgroup MATCH_STRUCT File matching structures
    \{ */

/** A structures that stores informations a file query */
typedef struct FILELIST {
	char  lpFileName[FILENAME_MAX]; /**< A pointer to a NULL-terminated string that stores the name of the file */
	struct stat stFileStats; /**< A structure that stores information about the file. Use \link FILEINFO_MACRO file information macros \endlink to get these informations */
	struct FILELIST* lpflNext; /**< A pointer to the list's next element */
} FILELIST,*LPFILELIST;

/** \} */

/** \defgroup MATCH_FUNCT File matching functions
    \{ */

/** \brief Determine wether a string match to an regular expression or not
    The functions uses dos-like regular expression, using wildcard '*' and joker '?'. The comparison is case sensitive.
    \param lpRegExp : A pointer to a NULL-terminated string that stores the regular expression
    \param lpMatch: A pointer to a NULL-terminated string that stores the string to be compared
    \return Return TRUE if lpMatch matches to lpRegExp, FALSE otherwise
    \attention The comparison is case-sensitive
*/
LIBDOS9 int         Dos9_RegExpMatch(const char* lpRegExp, const char* lpMatch);

/** \brief Determine wether a string match to an regular expression or not
    The functions uses dos-like regular expression, using wildcard '*' and joker '?'.
    \param lpRegExp : A pointer to a NULL-terminated string that stores the regular expression
    \param lpMatch: A pointer to a NULL-terminated string that stores the string to be compared
    \return Return TRUE if lpMatch matches to lpRegExp, FALSE otherwise */

LIBDOS9 int         Dos9_RegExpCaseMatch(const char* lpRegExp, const char* lpMatch);

/** \brief Retrieves a list of matching files
    \param lpPathMatch : A pointer to a NULL-terminated string that stores the regular expression to which the file have to match
    \param iFlag : A search flag. The flag must be one of the flag defined in \link MATCH_FLAG file matching flags \endlink
    \return Returns a pointer to a FILELIST structure. If the function fails, it returns NULL */
LIBDOS9 LPFILELIST  Dos9_GetMatchFileList(char* lpPathMatch, int iFlag);

LIBDOS9 int         Dos9_GetMatchFileCallback(char* lpPathMatch, int iFlag, void(*pCallBack)(FILELIST*));

/** \brief Free a FILELIST structure
    \param lpflFileList : A pointer to the file list to be freed
    \return Returns 0 on success or 1 on failure */
LIBDOS9 int         Dos9_FreeFileList(LPFILELIST lpflFileList);

LIBDOS9 int         Dos9_FormatFileSize (char* lpBuf, int iLenght, unsigned int iSize);
/** \} */
/** \} */

typedef struct {
	char bStat;
	int iInput;
	void(*pCallBack)(FILELIST*);
} FILEPARAMETER,*LPFILEPARAMETER;

char*               _Dos9_SeekPatterns(char* lpSearch, char* lpPattern);
char*               _Dos9_SeekCasePatterns(char* lpSearch, char* lpPattern);
int                 _Dos9_SplitMatchPath(const char* lpPathMatch, char* lpStaticPart, size_t iStaticSize,  char* lpMatchPart, size_t iMatchSize);
int                 _Dos9_GetMatchPart(const char* lpRegExp, char* lpBuffer, size_t iLength, int iLvl);
int                 _Dos9_GetMatchDepth(char* lpRegExp);
int                 _Dos9_MakePath(char* lpPathBase, char* lpPathEnd, char* lpBuffer, int iLength);
char*               _Dos9_GetFileName(char* lpPath);
int                 _Dos9_FreeFileList(LPFILELIST lpflFileList);
LPFILELIST          _Dos9_WaitForFileList(LPFILEPARAMETER lpParam);
int                 _Dos9_WaitForFileListCallBack(LPFILEPARAMETER lpParam);
LPFILELIST          _Dos9_SeekFiles(char* lpDir, char* lpRegExp, int iLvl, int iMaxLvl, int iOutDescriptor, int iSearchFlag);


/* declaration for attributes commands */

#define DOS9_CMD_ATTR_READONLY 0x01
#define DOS9_CMD_ATTR_READONLY_N 0x02
#define DOS9_CMD_ATTR_VOID 0x04
#define DOS9_CMD_ATTR_VOID_N 0x08
#define DOS9_CMD_ATTR_HIDEN 0x10
#define DOS9_CMD_ATTR_HIDEN_N 0x20
#define DOS9_CMD_ATTR_SYSTEM 0x40
#define DOS9_CMD_ATTR_SYSTEM_N 0x80
#define DOS9_CMD_ATTR_ARCHIVE 0x100
#define DOS9_CMD_ATTR_ARCHIVE_N 0x200
#define DOS9_CMD_ATTR_DIR 0x400
#define DOS9_CMD_ATTR_DIR_N 0x800
#define DOS9_CMD_ATTR_ALL 0x000

/** \defgroup FILE_ATTR_MATCH File attributes matching
    \{ */

/** \brief Converts an attribute token string specifier that contains letters for attributes, that support the same syntax as in /A[:]attr batch switchs into a short describing its attribute token
    \param lpToken : The string token to be converted
    \return Returns a short representing the attributes
*/
LIBDOS9 short Dos9_MakeFileAttributes(const char* lpToken);

/** \brief Checks wether a file returned by Dos9_GetMatchFileList() as attributes matching to a set of attributes
    \param wAttr : A short retrieved by Dos9_MakeFileAttributes().
    \param lpflList : A pointer to an element of a FILELIST structure retrieved by Dos9_GetMatchFileList()
    \return Returns 1 if the attributes match, 0 else
*/
LIBDOS9 int Dos9_CheckFileAttributes(short wAttr, const FILELIST* lpflList);

/** \} */
/* encoding declaration */

/** \defgroup ENCODING_REF Encoding references
    \{ */

/** \defgroup ENCODING_GLOBALS Encoding global vars
    \{ */

/** A global variable that store the encoding mode of the lib
    \attention Prefer use of Dos9_SetEncoding() to modify it */
extern int _Dos9_TextMode;

/** \} */

/** \defgroup ENCODING_MACRO Encoding macros
    \{ */

/** The encoding used is byte-based */
#define DOS9_BYTE_ENCODING 0

/** The encoding used is utf-8 */
#define DOS9_UTF8_ENCODING 1

/** Changes the encoding mode of the lib */
#define Dos9_SetEncoding(encoding) _Dos9_TextMode=encoding

/** \} */

/** \defgroup ENCODING_FUNC Encoding functions
    \{ */

/** \brief Get the addres of the begin of the next character consedering the encoding mode set
    \param lpContent : A pointer to a string character
    \return A pointer to the next string character
    \note if encoding is byte based, this is roughly equivalent to lpContent++ */
LIBDOS9 char* Dos9_GetNextChar(const char* lpContent);

/** \} */
/** \} */

int _Dos9_IsFollowingByte(const char* lpChar);

LIBDOS9 int Dos9_GetConsoleEncoding(char* lpEnc, size_t iSize);

LIBDOS9 int Dos9_FileExists(const char* lpPath);
LIBDOS9 int Dos9_DirExists(const char* lpPath);
LIBDOS9 int Dos9_UpdateCurrentDir(void);
LIBDOS9 int Dos9_SetCurrentDir(const char* lpPath);
LIBDOS9 char* Dos9_GetCurrentDir(void);
LIBDOS9 int Dos9_GetExePath(char* lpBuf, size_t iBufSize);



// LIBDOS9 int      Dos9_BeginThread(THREAD* lpThId, void(*pFunc)(void*), int iMemAmount, void* arg);
LIBDOS9 void     Dos9_EndThread(void* iReturn);
LIBDOS9 void     Dos9_AbortThread(THREAD * lpThId);
LIBDOS9 int      Dos9_WaitForThread(THREAD* thId, void* lpRet);
LIBDOS9 int      Dos9_WaitForAllThreads(int iDelay);
LIBDOS9 void     Dos9_AbortAllThreads(void);

LIBDOS9 int      Dos9_CreateMutex(MUTEX* lpMuId);
LIBDOS9 int      Dos9_CloseMutex(MUTEX* lpMuId);
LIBDOS9 int      Dos9_LockMutex(MUTEX* lpMuId);
LIBDOS9 int      Dos9_ReleaseMutex(MUTEX* lpMuId);

LIBDOS9 int      Dos9_CreateProcess(PROCESS* pId, char* lpName, char** lpArgs);
LIBDOS9 int      Dos9_WaitForProcess(PROCESS* pId, void* iRes);

/* skip blank characters ' ' and '\t' */
LIBDOS9 char* Dos9_SkipBlanks(const char* lpCh);

/* skip blanck characrers ' ' and '\t' and '@' and ';' */
LIBDOS9 char* Dos9_SkipAllBlanks(const char* lpCh);

/* search the first non escaped character */
LIBDOS9 char* Dos9_SearchChar(const char* lpCh, int cChar);

/* search the last non-escaped chararcter */
LIBDOS9 char* Dos9_SearchLastChar(const char* lpCh, int cChar);

/* un-escape the string */
LIBDOS9 void Dos9_UnEscape(char* lpCh);

LIBDOS9 char* Dos9_SearchToken(const char* lpCh, const char* lpDelims);

LIBDOS9 char* Dos9_SearchLastToken(const char* lpCh, const char* lpDelims);

LIBDOS9 char* Dos9_SearchChar_OutQuotes(const char* lpCh, int cChar);

LIBDOS9 char* Dos9_SearchToken_OutQuotes(const char* lpCh, const char* lpDelims);


/* end of libDos9 declaration */
#endif
