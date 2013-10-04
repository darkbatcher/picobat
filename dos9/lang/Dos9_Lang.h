#ifndef DOS9_LANG_INCLUDED_H
#define DOS9_LANG_INCLUDED_H



extern const char* lpMsgEchoOn;
extern const char* lpMsgEchoOff;
extern const char* lpMsgPause;

extern const char* lpDirNoFileFound;
extern const char* lpDirListTitle;
extern const char* lpDirFileDirNb;


extern const char* lpHlpMain;
extern const char* lpHlpDeprecated;

void Dos9_LoadStrings(void); /* a function that loads differents
                               languages traductions */

#endif
