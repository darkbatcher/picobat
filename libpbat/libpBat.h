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

#ifndef LIBPBAT_INCLUDED_H
#define LIBPBAT_INCLUDED_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "../config.h"

#ifdef  DLL_EXPORT
#define __LIBPBAT__DLL
#endif /* DLL_EXPORT */

#ifndef LIBPBAT
#ifdef __LIBPBAT__DLL
#define LIBPBAT __declspec(dllexport)
#define EXTERN extern
#else
#define LIBPBAT extern
#define EXTERN
#endif /* __LIBPBAT__DLL */
#endif /* LIBPBAT  */


#if defined PBAT_USE_LIBCU8
#include <libcu8.h>
#endif

#if defined WIN32

    #include <windows.h>
    #include <io.h>

    #define strdup pBat_strdup

    #define PBAT_FILE_ARCHIVE FILE_ATTRIBUTE_ARCHIVE
    #define PBAT_FILE_COMPRESSED FILE_ATTRIBUTE_COMPRESSED
    #define PBAT_FILE_HIDDEN FILE_ATTRIBUTE_HIDDEN
    #define PBAT_FILE_OFFLINE FILE_ATTRIBUTE_OFFLINE
    #define PBAT_FILE_READONLY FILE_ATTRIBUTE_READONLY
    #define PBAT_FILE_SYSTEM FILE_ATTRIBUTE_SYSTEM
    #define PBAT_FILE_DIR FILE_ATTRIBUTE_DIRECTORY

    /*
     * provide some wrappers over the windows-defined constants
     */
    #define O_WRONLY _O_WRONLY
    #define O_RDONLY _O_RDONLY
    #define O_TRUNC _O_TRUNC
    #define O_TEXT _O_TEXT
	#ifndef S_IRUSR
		#define S_IRUSR _S_IREAD
	#endif
	#ifndef S_IWUSR
		#define S_IWUSR _S_IWRITE
	#endif

    /*
     * provide some wrapper over windows pseudo-POSIX functions
     */

    #define write(a,b,c) _write(a,b,c)
    #define read(a,b,c) _read(a,b,c)
    #define access(a,b) _access(a,b)

    #define _pBat_Pipe(descriptors, size, mode) _pipe(descriptors, size, mode)
#else

    #include <pthread.h>

    #define PBAT_FILE_DIR S_IFDIR
    #define PBAT_FILE_EXECUTE S_IXUSR
    #define PBAT_FILE_READ S_IRUSR
    #define PBAT_FILE_WRITE S_IWUSR
    #define PBAT_FILE_REGULAR S_IFREG
    #define PBAT_FILE_ARCHIVE 0
    #define PBAT_FILE_COMPRESSED 0
    #define PBAT_FILE_HIDDEN 0
    #define PBAT_FILE_OFFLINE 0
    #define PBAT_FILE_READONLY 0
    #define PBAT_FILE_SYSTEM 0
    #define O_BINARY 0

    #define _pBat_Pipe(descriptors, size, mode) pipe(descriptors)

    #define stricmp strcasecmp
    #define strnicmp strncasecmp

#endif // WIN32

#ifdef WIN32

#include <conio.h>
#define PBAT_NL "\r\n"

#elif !defined(WIN32)

#include <strings.h>

#define stricmp strcasecmp
#define strnicmp strncasecmp

#define PBAT_NL "\n"

#endif

/* define TRUE and FALSE CONSTANTS if they are not
   defined yet */

#ifndef TRUE
#define TRUE 1
#endif // TRUE

#ifndef FALSE
#define FALSE 0
#endif // TRUE


#ifdef WIN32

typedef HANDLE THREAD;
typedef CRITICAL_SECTION MUTEX;
typedef int    PROCESS;

#else

struct pbat_thread_t {
    pthread_t thread;
    int joined;
};

typedef struct pbat_thread_t   THREAD;
typedef pthread_mutex_t MUTEX;
typedef pid_t           PROCESS;

#endif

LIBPBAT int      pBat_BeginThread(THREAD* lpThId, void*(*pFunc)(void*),
					int iMemAmount, void* arg);
LIBPBAT void     pBat_EndThread(void* iReturn);
LIBPBAT void     pBat_AbortThread(THREAD* lpThId);
LIBPBAT int      pBat_WaitForThread(THREAD* thId, void** pRet);
LIBPBAT void     pBat_CloseThread(THREAD* thId);

LIBPBAT int      pBat_CreateMutex(MUTEX* lpMuId);
LIBPBAT int      pBat_CloseMutex(MUTEX* lpMuId);
LIBPBAT int      pBat_LockMutex(MUTEX* lpMuId);
LIBPBAT int      pBat_ReleaseMutex(MUTEX* lpMuId);

/* exports initialisation functions */
LIBPBAT 	int pBat_LibInit(void);
LIBPBAT 	void pBat_LibClose(void);

typedef struct STACK {
    int iFlag;
    void* ptrContent;
    struct STACK* lpcsPrevious;
} STACK,*LPSTACK;

LIBPBAT     LPSTACK pBat_PushStack(LPSTACK lpcsStack, void* ptrContent);
LIBPBAT     LPSTACK pBat_PopStack(LPSTACK lpcsStack, void(*pFunc)(void*));
LIBPBAT     int pBat_GetStack(LPSTACK lpcsStack, void** ptrContent);
LIBPBAT     int pBat_ClearStack(LPSTACK lpcsStack, void(*pFunc)(void*));

typedef struct ESTR {
    char* str;
    int len;
} ESTR, *LPESTR;

#define EsToChar(a) a->str
#define pBat_EsToChar(a) a->str

#define PBAT_NEWLINE_WINDOWS 0
#define PBAT_NEWLINE_UNIX 1
#define PBAT_NEWLINE_LINUX PBAT_NEWLINE_UNIX
#define PBAT_NEWLINE_MAC 2

#define pBat_SetNewLineMode(type) _pBat_NewLine=type

EXTERN LIBPBAT int      _pBat_NewLine;
LIBPBAT ESTR*           pBat_EsInit(void);
LIBPBAT char*           pBat_strdup(const char* src);
LIBPBAT int             pBat_EsGet(ESTR* ptrESTR, FILE* ptrFile);
LIBPBAT int             pBat_EsCpy(ESTR* ptrESTR, const char* ptrChaine);
LIBPBAT int             pBat_EsCpyN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize);
LIBPBAT int             pBat_EsCat(ESTR* ptrESTR, const char* ptrChaine);
LIBPBAT int             pBat_EsCatN(ESTR* ptrESTR, const char* ptrChaine, size_t iSize);
LIBPBAT int             pBat_EsFree(ESTR* ptrESTR);
LIBPBAT int             pBat_EsCpyE(ESTR* ptrSource, const ESTR* ptrDest);
LIBPBAT int             pBat_EsCatE(ESTR* ptrDest, const ESTR* ptrSource);
LIBPBAT int             pBat_EsReplace(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace);
LIBPBAT int             pBat_EsReplaceN(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace, int iN);
LIBPBAT int pBat_EsReplaceI(ESTR* ptrESTR, const char* ptrPattern, const char* ptrReplace);


typedef int COMMANDFLAG;

typedef struct COMMANDLIST
{
    void* lpCommandProc;
    int iLenght;
    char* ptrCommandName;
    COMMANDFLAG cfFlag;
    struct COMMANDLIST* lpclLeftRoot;
    struct COMMANDLIST* lpclRightRoot;
} COMMANDLIST, *LPCOMMANDLIST;

typedef struct COMMANDINFO {
    char* ptrCommandName;
    void* lpCommandProc;
    COMMANDFLAG cfFlag;
} COMMANDINFO,*LPCOMMANDINFO;

#define PBAT_DEF_FLAG 0x80000000
#define PBAT_COMMAND_LOOKAHEAD 0x40000000
#define PBAT_COMMAND_FLAGS (PBAT_DEF_FLAG | PBAT_COMMAND_LOOKAHEAD)

LIBPBAT LPCOMMANDLIST   pBat_MapCommandInfo(LPCOMMANDINFO lpciCommandInfo, int i);
LIBPBAT LPCOMMANDLIST   pBat_ReMapCommandInfo(LPCOMMANDLIST lpclCommandList);
LIBPBAT LPCOMMANDLIST   pBat_DuplicateCommandList(LPCOMMANDLIST lpclList);
LIBPBAT int             pBat_AddCommandDynamic(LPCOMMANDINFO lpciCommandInfo, LPCOMMANDLIST* lpclListEntry);
LIBPBAT int				pBat_ReplaceCommand(LPCOMMANDINFO lpciCommand, LPCOMMANDLIST lpclCommandList);
LIBPBAT int             pBat_FreeCommandList(LPCOMMANDLIST lpclList);
LIBPBAT COMMANDFLAG     pBat_GetCommandProc(char* lpCommandLine, LPCOMMANDLIST lpclCommandList,void** lpcpCommandProcedure);


#define PBAT_CURSOR_SHOW 0
#define PBAT_CURSOR_HIDE 1

#if defined(WIN32) && !defined(LIBPBAT_W10_ANSI)
    #define PBAT_FOREGROUND_RED FOREGROUND_RED
    #define PBAT_FOREGROUND_BLUE FOREGROUND_BLUE
    #define PBAT_FOREGROUND_GREEN FOREGROUND_GREEN
    #define PBAT_BACKGROUND_GREEN BACKGROUND_GREEN
    #define PBAT_BACKGROUND_BLUE BACKGROUND_BLUE
    #define PBAT_BACKGROUND_RED BACKGROUND_RED

    #define PBAT_BACKGROUND_INT BACKGROUND_INTENSITY
    #define PBAT_FOREGROUND_INT FOREGROUND_INTENSITY

    #define PBAT_BACKGROUND_DEFAULT 0
    #define PBAT_FOREGROUND_DEFAULT PBAT_FOREGROUND_GREEN | PBAT_FOREGROUND_RED | PBAT_FOREGROUND_BLUE

    #define PBAT_GET_FOREGROUND(a) (a & 0x0F)
    #define PBAT_GET_FOREGROUND_(a) (a & 0x0F)
    #define PBAT_GET_BACKGROUND(a) (a & 0xF0)
    #define PBAT_GET_BACKGROUND_(a) ((a & 0xF0)>>4)

#else

    #define PBAT_FOREGROUND_RED 0x01
    #define PBAT_FOREGROUND_BLUE 0x04
    #define PBAT_FOREGROUND_GREEN 0x02
    #define PBAT_BACKGROUND_GREEN 0x20
    #define PBAT_BACKGROUND_BLUE 0x40
    #define PBAT_BACKGROUND_RED 0x10

    #define PBAT_BACKGROUND_INT 0x80
    #define PBAT_FOREGROUND_INT 0x08

    #define PBAT_BACKGROUND_DEFAULT 0x0100
    #define PBAT_FOREGROUND_DEFAULT 0x0200

    #define PBAT_GET_FOREGROUND(a) (a & 0x20F)
    #define PBAT_GET_FOREGROUND_(a) (a & 0x07)
    #define PBAT_GET_BACKGROUND(a) (a & 0x1F0)
    #define PBAT_GET_BACKGROUND_(a) ((a & 0xF0)>>4)


#endif

#define PBAT_BACKGROUND_IBLUE PBAT_BACKGROUND_BLUE | PBAT_BACKGROUND_INT
#define PBAT_BACKGROUND_IRED PBAT_BACKGROUND_RED | PBAT_BACKGROUND_INT
#define PBAT_BACKGROUND_IGREEN PBAT_BACKGROUND_GREEN | PBAT_BACKGROUND_INT
#define PBAT_BACKGROUND_YELLOW PBAT_BACKGROUND_RED | PBAT_BACKGROUND_GREEN
#define PBAT_BACKGROUND_CYAN PBAT_BACKGROUND_BLUE | PBAT_BACKGROUND_GREEN
#define PBAT_BACKGROUND_MAGENTA PBAT_BACKGROUND_BLUE | PBAT_BACKGROUND_RED
#define PBAT_BACKGROUND_WHITE PBAT_BACKGROUND_BLUE | PBAT_BACKGROUND_RED | PBAT_BACKGROUND_GREEN
#define PBAT_BACKGROUND_BLACK 0
#define PBAT_BACKGROUND_IYELLOW PBAT_BACKGROUND_RED | PBAT_BACKGROUND_GREEN | PBAT_BACKGROUND_INT
#define PBAT_BACKGROUND_ICYAN PBAT_BACKGROUND_BLUE | PBAT_BACKGROUND_GREEN | PBAT_BACKGROUND_INT
#define PBAT_BACKGROUND_IMAGENTA PBAT_BACKGROUND_BLUE | PBAT_BACKGROUND_RED | PBAT_BACKGROUND_INT
#define PBAT_BACKGROUND_IWHITE PBAT_BACKGROUND_BLUE | PBAT_BACKGROUND_RED | PBAT_BACKGROUND_GREEN | PBAT_BACKGROUND_INT
#define PBAT_BACKGROUND_IBLACK PBAT_BACKGROUND_INT
#define PBAT_FOREGROUND_YELLOW PBAT_FOREGROUND_RED | PBAT_FOREGROUND_GREEN
#define PBAT_FOREGROUND_CYAN PBAT_FOREGROUND_BLUE | PBAT_FOREGROUND_GREEN
#define PBAT_FOREGROUND_MAGENTA PBAT_FOREGROUND_BLUE | PBAT_FOREGROUND_RED
#define PBAT_FOREGROUND_WHITE PBAT_FOREGROUND_BLUE | PBAT_FOREGROUND_RED | PBAT_FOREGROUND_GREEN
#define PBAT_FOREGROUND_BLACK 0
#define PBAT_FOREGROUND_IBLUE PBAT_FOREGROUND_BLUE | PBAT_FOREGROUND_INT
#define PBAT_FOREGROUND_IRED PBAT_FOREGROUND_RED | PBAT_FOREGROUND_INT
#define PBAT_FOREGROUND_IGREEN PBAT_FOREGROUND_GREEN | PBAT_FOREGROUND_INT
#define PBAT_FOREGROUND_IYELLOW PBAT_FOREGROUND_RED | PBAT_FOREGROUND_GREEN | PBAT_FOREGROUND_INT
#define PBAT_FOREGROUND_ICYAN PBAT_FOREGROUND_BLUE | PBAT_FOREGROUND_GREEN | PBAT_FOREGROUND_INT
#define PBAT_FOREGROUND_IMAGENTA PBAT_FOREGROUND_BLUE | PBAT_FOREGROUND_RED | PBAT_FOREGROUND_INT
#define PBAT_FOREGROUND_IWHITE PBAT_FOREGROUND_BLUE | PBAT_FOREGROUND_RED | PBAT_FOREGROUND_GREEN | PBAT_FOREGROUND_INT
#define PBAT_FOREGROUND_IBLACK PBAT_FOREGROUND_INT
#define PBAT_COLOR_DEFAULT PBAT_BACKGROUND_DEFAULT | PBAT_FOREGROUND_DEFAULT

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

enum {
  CORE_NOTHING = 0,
  CORE_LEFT_BUTTON,
  CORE_RIGHT_BUTTON,
  CORE_D_LEFT_BUTTON,
  CORE_D_RIGHT_BUTTON,
  CORE_MIDDLE_BUTTON,
  CORE_SCROLL_UP,
  CORE_SCROLL_DOWN,
  CORE_RELEASE
};

/* core_i special keys */
enum {
  CORE_KEY_ERROR = -1,
  CORE_KEY_UNKNOWN = -1, /* same as CORE_KEY_ERROR */
  CORE_KEY_MOUSE = -2, /* *NIX only */
};

LIBPBAT void            pBat_ClearConsoleScreen(FILE* f);
LIBPBAT void            pBat_ClearConsoleLine(FILE* f);
LIBPBAT void            pBat_SetConsoleColor(FILE* f, COLOR cColor);
LIBPBAT void            pBat_SetConsoleTextColor(FILE* f, COLOR cColor);
LIBPBAT void            pBat_SetConsoleCursorPosition(FILE* f, CONSOLECOORD iCoord);
LIBPBAT CONSOLECOORD    pBat_GetConsoleCursorPosition(FILE* f);
LIBPBAT void            pBat_SetConsoleCursorState(FILE* f, int bVisible, int iSize);
LIBPBAT void            pBat_SetConsoleTitle(FILE* f, char* lpTitle);
LIBPBAT int             pBat_Kbhit(FILE* f);
LIBPBAT int             pBat_Getch(FILE* f);
LIBPBAT void            pBat_GetMousePos(FILE* f, char on_move, CONSOLECOORD* coords, int* type);

#define pBat_GetAccessTime(lpList) (lpList->stFileStats.st_atime)
#define pBat_GetCreateTime(lpList) (lpList->stFileStats.st_ctime)
#define pBat_GetModifTime(lpList) (lpList->stFileStats.st_mtime)
#define pBat_GetFileSize(lpList) (lpList->stFileStats.st_size)
#define pBat_GetFileMode(lpList) (lpList->stFileStats.st_mode)

/* Match everything in the directory */
#define PBAT_SEARCH_DEFAULT 0x00
/* Browse sub-dir for matches */
#define PBAT_SEARCH_RECURSIVE 0x01
/* Stop after the first match is found */
#define PBAT_SEARCH_GET_FIRST_MATCH 0x02
/* Do not grab informations about files */
#define PBAT_SEARCH_NO_STAT 0x04
/* Do not list pseudo dirs '.' and '..' */
#define PBAT_SEARCH_NO_PSEUDO_DIR 0x08
// Search mode equivalent DIR (ie "dir" is equivalent to "dir/*
#define PBAT_SEARCH_DIR_MODE 0x10

#ifndef WIN32
#define PBAT_TEST_SEPARATOR(p) (*(p) == '/')
#define PBAT_TEST_ABSOLUTE_PATH(p) PBAT_TEST_ROOT_PATH(p)
#define PBAT_TEST_UNC_PATH(p)  0
#define PBAT_TEST_ROOT_PATH(p) (PBAT_TEST_SEPARATOR(p) && !PBAT_TEST_SEPARATOR(p+1))
#define PBAT_TEST_DRIVE_PATH(p) 0
#define PBAT_DEF_DELIMITER "/"
#define PBAT_DEF_DELIMITERS "/"
#define PBAT_DEF_SEPARATOR '/'
#else
/* The path may refer to a unc-type path */
#define PBAT_TEST_SEPARATOR(p) (*(p) == '/' || *(p) == '\\')
#define PBAT_TEST_UNC_PATH(p)  (*(p) == '\\' && *(p+1) == '\\')
#define PBAT_TEST_ROOT_PATH(p) (PBAT_TEST_SEPARATOR(p) && !PBAT_TEST_SEPARATOR(p+1))
#define PBAT_TEST_DRIVE_PATH(p) (*(p) && *(p+1)==':' && PBAT_TEST_SEPARATOR(p+2))
#define PBAT_TEST_ABSOLUTE_PATH(p) (PBAT_TEST_UNC_PATH(p) || PBAT_TEST_ROOT_PATH(p) \
                                || PBAT_TEST_DRIVE_PATH(p))
#define PBAT_DEF_DELIMITER "\\"
#define PBAT_DEF_DELIMITERS "\\/"
#define PBAT_DEF_SEPARATOR '\\'

#define PBAT_TEST_SEPARATOR_W(p) (*(p) == L'/' || *(p) == L'\\')
#define PBAT_TEST_UNC_PATH_W(p)  (*(p) == L'\\' && *(p+1) == L'\\')
#define PBAT_TEST_ROOT_PATH_W(p) (PBAT_TEST_SEPARATOR_W(p) && !PBAT_TEST_SEPARATOR_W(p+1))
#define PBAT_TEST_DRIVE_PATH_W(p) (*(p) && *(p+1)==':' && PBAT_TEST_SEPARATOR_W(p+2))
#define PBAT_TEST_ABSOLUTE_PATH_W(p) (PBAT_TEST_UNC_PATH_W(p) || PBAT_TEST_ROOT_PATH_W(p) \
                                || PBAT_TEST_DRIVE_PATH_W(p))
#define PBAT_DEF_DELIMITER_W L"\\"
#define PBAT_DEF_DELIMITERS_W L"\\/"
#define PBAT_DEF_SEPARATOR_W L'\\'
#endif // _POSIX_C_SOURCE


typedef struct FILELIST {
    char  lpFileName[FILENAME_MAX]; /* filename */
    struct stat stFileStats; /* file stats */
    struct FILELIST* lpflNext; /* pointer to next match */
    struct FILELIST* lpflPrevious; /* Use only internally, do not use this */
} FILELIST,*LPFILELIST;

LIBPBAT int         pBat_RegExpMatch(const char* restrict lpRegExp, const char* restrict lpMatch);
LIBPBAT int         pBat_RegExpCaseMatch(const char* restrict lpRegExp, const char* restrict lpMatch);
LIBPBAT LPFILELIST  pBat_GetMatchFileList(char* lpPathMatch, int iFlag);
LIBPBAT int         pBat_GetMatchFileCallback(char* lpPathMatch, int iFlag, void(*pCallBack)(FILELIST*));
LIBPBAT int         pBat_FreeFileList(LPFILELIST lpflFileList);
LIBPBAT int         pBat_FormatFileSize (char* lpBuf, int iLenght, unsigned int iSize);
LIBPBAT int         pBat_GetStaticPart(const char* lpPathMatch, char* lpStaticPart, size_t size);
LIBPBAT size_t      pBat_GetStaticLength(const char* str);

#define PBAT_CMD_ATTR_READONLY 0x01
#define PBAT_CMD_ATTR_READONLY_N 0x02
#define PBAT_CMD_ATTR_VOID 0x04
#define PBAT_CMD_ATTR_VOID_N 0x08
#define PBAT_CMD_ATTR_HIDEN 0x10
#define PBAT_CMD_ATTR_HIDEN_N 0x20
#define PBAT_CMD_ATTR_SYSTEM 0x40
#define PBAT_CMD_ATTR_SYSTEM_N 0x80
#define PBAT_CMD_ATTR_ARCHIVE 0x100
#define PBAT_CMD_ATTR_ARCHIVE_N 0x200
#define PBAT_CMD_ATTR_DIR 0x400
#define PBAT_CMD_ATTR_DIR_N 0x800
#define PBAT_CMD_ATTR_ALL 0x000

#define PBAT_ATTR_DIR         (PBAT_CMD_ATTR_DIR)
#define PBAT_ATTR_NO_DIR      (PBAT_CMD_ATTR_DIR | PBAT_CMD_ATTR_DIR_N)
#define PBAT_ATTR_VOID        (PBAT_CMD_ATTR_VOID)
#define PBAT_ATTR_NO_VOID     (PBAT_CMD_ATTR_VOID | PBAT_CMD_ATTR_VOID_N)
#define PBAT_ATTR_HIDEN       (PBAT_CMD_ATTR_HIDEN)
#define PBAT_ATTR_NO_HIDEN    (PBAT_CMD_ATTR_HIDEN | PBAT_CMD_ATTR_HIDEN_N)
#define PBAT_ATTR_READONLY    (PBAT_CMD_ATTR_READONLY)
#define PBAT_ATTR_NO_READONLY (PBAT_CMD_ATTR_READONLY | PBAT_CMD_ATTR_READONLY_N)
#define PBAT_ATTR_SYSTEM      (PBAT_CMD_ATTR_SYSTEM)
#define PBAT_ATTR_NO_SYSTEM   (PBAT_CMD_ATTR_SYSTEM | PBAT_CMD_ATTR_SYSTEM_N)
#define PBAT_ATTR_ARCHIVE     (PBAT_CMD_ATTR_ARCHIVE)
#define PBAT_ATTR_NO_ARCHIVE  (PBAT_CMD_ATTR_ARCHIVE | PBAT_CMD_ATTR_ARCHIVE_N)
#define PBAT_ATTR_ALL         (PBAT_CMD_ATTR_ALL)

LIBPBAT short pBat_MakeFileAttributes(const char* lpToken);
LIBPBAT int pBat_CheckFileAttributes(short wAttr, const FILELIST* lpflList);
LIBPBAT int pBat_AttributesSplitFileList(short wAttr, FILELIST* pIn,
                            FILELIST** pSelected, FILELIST** pRemaining);

EXTERN LIBPBAT int _pBat_TextMode;
#define PBAT_BYTE_ENCODING 0
#define PBAT_UTF8_ENCODING 1
#define pBat_SetEncoding(encoding) _pBat_TextMode=encoding

#if defined(WIN32)
#define pBat_Sleep(ms) Sleep(ms)
#else
void pBat_Sleep(unsigned int ms);
#endif

LIBPBAT char* pBat_GetNextChar(const char* lpContent);
LIBPBAT int pBat_GetConsoleEncoding(char* lpEnc, size_t iSize);
LIBPBAT const char* pBat_ConsoleCP2Encoding(int cp);

LIBPBAT int pBat_FileExists(const char* lpPath);
LIBPBAT char* pBat_GetFirstExistingFile(char** files);
LIBPBAT int pBat_DirExists(const char* lpPath);
LIBPBAT int pBat_SetFileMode(const char* file, int attr);
LIBPBAT int pBat_GetExePath(char* lpBuf, size_t iBufSize);
LIBPBAT int pBat_GetExeFilename(char* lpBuf, size_t iBuffSize);

#define pBat_IsDelim(ch) \
    ((ch==' ') || (ch=='\t') || (ch==',') || (ch==';') || (ch=='\n'))

LIBPBAT char* pBat_SkipBlanks(const char* lpCh);
LIBPBAT char* pBat_SkipAllBlanks(const char* lpCh);
LIBPBAT char* pBat_SearchChar(const char* lpCh, int cChar);
LIBPBAT char* pBat_SearchLastChar(const char* lpCh, int cChar);
LIBPBAT void  pBat_UnEscape(char* lpCh);
LIBPBAT char* pBat_GetNextNonEscaped(const char* lpCh);
LIBPBAT char* pBat_SearchToken(const char* restrict lpCh, const char* restrict lpDelims);
LIBPBAT char* pBat_SearchLastToken(const char* restrict lpCh, const char* restrict lpDelims);
LIBPBAT char* pBat_SearchChar_OutQuotes(const char* lpCh, int cChar);
LIBPBAT char* pBat_SearchToken_OutQuotes(const char* restrict lpCh, const char* restrict lpDelims);
LIBPBAT char* pBat_SearchToken_Hybrid(const char* restrict pch, const char* restrict delims,
                                                            const char* restrict qdelims);
LIBPBAT void pBat_StripEndDelims(char* str);

#endif // LIBPBAT_INCLUDED_H
