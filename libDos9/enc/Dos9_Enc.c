#include "../libDos9-int.h"

int _Dos9_TextMode=DOS9_BYTE_ENCODING;

#define UNICODE_BYTE 0x80
#define UNICODE_FOLOWING_BYTE_MASK 0xC0
#define UNICODE_FOLOWING_BYTE_MARK 0x80

int _Dos9_IsFollowingByte(const char* lpChar)
{
    if ( ((*lpChar) & UNICODE_FOLOWING_BYTE_MASK) == UNICODE_FOLOWING_BYTE_MARK ) {

        return TRUE;

    } else {

        return FALSE;

    }
}

LIBDOS9 char* Dos9_GetNextChar(const char* lpContent)
{

    if (_Dos9_TextMode==DOS9_UTF8_ENCODING) {
        /* système de gestion des caractères UTF-8 */

        if (!(*lpContent & UNICODE_BYTE)) {
            /* il s'agit d'un caractère de la norme ASCII */
            return (char*)lpContent+1;
        }

        /* sinon on boucle pour parvenir au prochain caractère */

        lpContent++;

        /* on va jusqu'a prochain octet non suivant */
        while (_Dos9_IsFollowingByte(lpContent)==TRUE && *lpContent) {

            lpContent++;

        }

        return (char*)lpContent;

    } else {

        return (char*)lpContent+1;

    }
}

#ifdef WIN32

#include <windows.h>

typedef struct ENCODINGINFO {
    int iCodePage;
    char* lpCodePageName;
} ENCODINGINFO;

const ENCODINGINFO EncodingInfo[] = {
                                        {20127, "ASCII"},
                                        {65001, "UTF-8"},
                                        {65000, "UTF-7"},
                                        {1200, "UTF-16LE"},
                                        {1201, "UTF-16BE"},
                                        {12000, "UTF-32LE"},
                                        {12001, "UTF-32BE"},
                                        {28591, "ISO-8859-1"}, // list of iso latin charset
                                        {28592, "ISO-8859-2"},
                                        {28593, "ISO-8859-3"},
                                        {28594, "ISO-8859-4"},
                                        {28595, "ISO-8859-5"},
                                        {28596, "ISO-8859-6"},
                                        {28597, "ISO-8859-7"},
                                        {28598, "ISO-8859-8"},
                                        {28599, "ISO-8859-9"},
                                        {28603, "ISO-8859-13"},
                                        {28603, "ISO-8859-13"},
                                        {28603, "ISO-8859-15"},
                                        {20866, "KIO8-RU"}, // russian encodings
                                        {21866, "KIO8-U"},  // ukranian
                                        {1250, "WINDOWS-1250"}, // windows latin
                                        {1251, "WINDOWS-1251"},
                                        {1252, "WINDOWS-1252"},
                                        {1253, "WINDOWS-1253"},
                                        {1254, "WINDOWS-1254"},
                                        {1255, "WINDOWS-1255"},
                                        {1256, "WINDOWS-1256"},
                                        {1257, "WINDOWS-1257"},
                                        {1258, "WINDOWS-1258"},
                                        {874, "WINDOWS-874"},
                                        {850, "CP850"},
                                        {862, "CP862"},
                                        {866, "CP866"},
                                        {10079, "MACICELAND"},
                                        {10082, "MACCROATIAN"},
                                        {10083, "MACTURKISH"},
                                        {10010, "MACROMANIA"},
                                        {10007, "MACYRILIC"},
                                        {10017, "MACUKRAINE"},
                                        {10006, "MACGREEK"},
                                        {10005, "MACHEBREW"},
                                        {10004, "MACARABIC"},
                                        {10021, "MACTHAI"},
                                        {50222, "JISX0201-1976"},
                                        {10008, "CHINESE"},
                                        {51932, "EUC-JP"},
                                        {932 , "SHIFT-JIS"},
                                        {50220 , "iso-2022-jp"},
                                        {51936 , "EUC-CN"},
                                        {54936 , "GB18030"},
                                        {52936 , "HZ-GB-2312"},
                                        {950 , "BIG5"},
                                        {51949 , "EUC-KR"},
                                        {1361 , "CP1361"},
                                        {949 , "UHC"},
                                        {50225 , "ISO-2020-KR"},
                                        {856, "CP856"},
                                        {922, "CP922"},
                                        {943, "CP943"},
                                        {1046, "CP1046"},
                                        {1124, "CP1124"},
                                        {1129, "CP1129"},
                                        {1161, "CP1161"},
                                        {1162, "CP1162"},
                                        {1163, "CP1163"},
                                        {437, "CP437"},
                                        {737, "CP737"},
                                        {775, "CP775"},
                                        {852, "CP852"},
                                        {853, "CP853"},
                                        {855, "CP855"},
                                        {857, "CP857"},
                                        {858, "CP858"},
                                        {860, "CP860"},
                                        {861, "CP861"},
                                        {863, "CP863"},
                                        {864, "CP864"},
                                        {865, "CP865"},
                                        {869, "CP869"}
};


int Dos9_GetConsoleEncoding(char* lpEnc, size_t iSize)
{
    int i;
    int iCodePageId;
    char* lpResult=NULL;

    iCodePageId=GetConsoleOutputCP();

    lpResult = Dos9_ConsoleCP2Encoding(iCodePageId);

    if (lpResult) {

        strncpy(lpEnc, lpResult, iSize);
        lpEnc[iSize-1]='\0';
        return 0;

    } else {

        return -1;

    }

}

const char* Dos9_ConsoleCP2Encoding(int cp)
{
    int i;

    for (i=0;i<sizeof(EncodingInfo)/sizeof(ENCODINGINFO);i++) {

        if (cp == EncodingInfo[i].iCodePage)
            return EncodingInfo[i].lpCodePageName;

    }

    return NULL;
}

#else

    int Dos9_GetConsoleEncoding(char* lpEnc, size_t iSize)
    {
        snprintf(lpEnc, iSize, "%s", "UTF-8");

        return 0;
    }

#endif // WINDOWS
