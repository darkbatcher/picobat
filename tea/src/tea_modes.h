#ifndef TEA_MODE_H
#define TEA_MODE_H

#include "libDos9.h"
#include "Tea_Lib.h"

extern TEAMODIFIERS teaTextMods;
extern TEAOUTPUT teaTextOutput;

extern TEAMODIFIERS teaHTMLMods;
extern TEAOUTPUT teaHTMLOutput;
extern char* lpTeaHTMLFrontEnd;

extern TEAMODIFIERS teaHlpMods;
extern TEAOUTPUT teaHlpOutput;
extern char* lpTeaHlpFrontEnd;

extern ESTR* lpFrontEndCommandLine;

#endif // TEA_MODE_H
