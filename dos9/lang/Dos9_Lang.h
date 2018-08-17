/*
 *
 *   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
 *   Copyright (C) 2010-2015 Romain GARBI
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

#ifndef DOS9_LANG_INCLUDED_H
#define DOS9_LANG_INCLUDED_H



extern const char* lpMsgEchoOn;
extern const char* lpMsgEchoOff;
extern const char* lpMsgPause;

extern const char* lpDirNoFileFound;
extern const char* lpDirListTitle;
extern const char* lpDirFile;
extern const char* lpDirDir;


extern const char* lpHlpMain;
extern const char* lpHlpDeprecated;

extern const char* lpDelConfirm;
extern const char* lpRmdirConfirm;
extern const char* lpCopyConfirm;

extern const char* lpBreakConfirm;

extern const char* lpAskYn;
extern const char* lpAskyN;
extern const char* lpAskyn;

extern const char* lpAskYna;
extern const char* lpAskyNa;
extern const char* lpAskynA;
extern const char* lpAskyna;

extern const char* lpAskYes;
extern const char* lpAskYesA;

extern const char* lpAskNo;
extern const char* lpAskNoA;

extern const char* lpAskAll;
extern const char* lpAskAllA;

extern const char* lpAskInvalid;

extern const char* lpManyCompletionOptions;

extern const char* lpMsgTimeout;
extern const char* lpMsgTimeoutNoBreak;
extern const char* lpMsgTimeoutBreak;
extern const char* lpMsgTimeoutKeyPress;


void Dos9_LoadStrings(void); /* a function that loads differents
                               languages traductions */

#endif
