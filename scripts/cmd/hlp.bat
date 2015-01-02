@ECHO Off
SETLOCAL EnableDelayedExpansion

::   Dos9 - A Free, Cross-platform command prompt - The Dos9 project
::   Copyright (C) 2010-2015 DarkBatcher
::
::   This program is free software: you can redistribute it and/or modify
::   it under the terms of the GNU General Public License as published by
::   the Free Software Foundation, either version 3 of the License, or
::   (at your option) any later version.
::
::   This program is distributed in the hope that it will be useful,
::   but WITHOUT ANY WARRANTY; without even the implied warranty of
::   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
::   GNU General Public License for more details.
::
::   You should have received a copy of the GNU General Public License
::   along with this program.  If not, see <http://www.gnu.org/licenses/>.

if /i "%~1" equ "/l" goto :list
if /i "%~1" equ "/b" goto :build
if /i "%~1" neq "/?" goto :show
:: if nothing has been selected, goto show

ECHO Hlp - Manual page viewier Copyright ^(C^) 2013-2014 Romain Garbi ^(Darkbatcher^)
ECHO This program comes with ABSOLUTELY NO WARRANTY; This is free software,
ECHO and you are welcome to redistribute it under the terms of the GNU CPL.
ECHO.
ECHO Hlp [page-name ^| /b [language [charset]] ^| /l ^| /d]
ECHO.
ECHO     - page-name : The page to be opened
ECHO.
ECHO     - /b : build the manual pages using given charset and language: 
ECHO.
ECHO        - charset : The charset to be used to build the manual pages. If no
ECHO          charset is given, then it will use the default charset ^(basically,
ECHO          the utf-8 charset^). This require the ICONV command to support. Use
ECHO          this parameter if your terminal does not support basic utf-8 encoding
ECHO          ^(like windows^).
ECHO.
ECHO        - language : The language to be used to build the manual page.
ECHO.
ECHO     - /l : list the languages availiable for the manual page maker.
ECHO.
ECHO     - /d : download the manual package from the repository at
ECHO       ^<http://www.dos9.org^>.     

GOTO:EOF

:build
ECHO Hlp - Manual page viewier Copyright ^(C^) 2013-2014 Romain Garbi ^(Darkbatcher^)
ECHO This program comes with ABSOLUTELY NO WARRANTY; This is free software,
ECHO and you are welcome to redistribute it under the terms of the GNU CPL.
ECHO.

cd %DOS9_PATH%/share
shift /1
IF "%~1" equ "" (

   IF defined LANGUAGE (
  
	  SET lang_build=!LANGUAGE!
   
   ) else (

	  SET lang_build=en_US

   )

) else (

   SET lang_build=%~1
   shift /1
)

IF "%~1" equ "" (

	SET charset_build=UTF-8

) else (

	SET charset_build=%~1

)

ECHO *** Building manual pages for %lang_build% with charset %charset_build% ***
ECHO.
ECHO  1] Remove content of the documentation folder.
ECHO.
ECHO  2] Creating subdirectories.

:: create static directory
for %%A in (doc
doc/ansi
doc/html
) do (

	IF not exist %%A mkdir %%A

)

:: browse the directory and make subirectories that
:: are not existing.
FOR /F "tokens=*" %%A in ('dir /b /s /a:d man/%lang_build%') do (
	
	SET path_build=%%~dpnA
	
	SET path_ansi_build=!path_build:man/%lang_build%=doc/ansi!
	SET path_html_build=!path_build:man/%lang_build%=doc/html!
	
	IF not exist !path_ansi_build! mkdir !path_ansi_build!
	IF not exist !path_html_build! mkdir !path_html_build!

)

ECHO.
ECHO. 3] Produce the output :
:: browse the directory and use tea to produce the output.
FOR /F "tokens=*" %%A in ('dir /b /s /a:-d man/%lang_build%') do (
	
	SET path_build=%%~dpnA
	
	ECHO    [*] Building from "%%A" "!path_build!" :
	
	SET path_ansi_build=!path_build:man/%lang_build%=doc/ansi!.ansi
	SET path_html_build=!path_build:man/%lang_build%=doc/html!.html
	
	ECHO      - file : !path_ansi_build!
	tea %%A !path_ansi_build!.tmp /E:UTF-8 /O:TEXT-ANSI 
	
	ECHO      - file : !path_html_build!
    	tea %%A !path_html_build! /E:UTF-8 /O:html /T "The Dos9 Project :: "
	
	ECHO      - converting output from UTF-8 to %charset_build%
	
	:: just convert .ansi file, it is not needed for html document
	:: which were generated using tea, that means to change encoding
	:: and tag <meta charset="utf-8" />
	iconv -f UTF-8 -t %charset_build% !path_ansi_build!.tmp > !path_ansi_build!

)

GOTO:EOF


:list
ECHO Hlp - Manual page viewier Copyright ^(C^) 2013-2014 Romain Garbi ^(Darkbatcher^)
ECHO This program comes with ABSOLUTELY NO WARRANTY; This is free software,
ECHO and you are welcome to redistribute it under the terms of the GNU CPL.
ECHO.
ECHO This is a list of avaliable languages :
ECHO Files are located in "%DOS9_PATH%/share/man"
ECHO.
CD %DOS9_PATH%/share/man
DIR /a:D /b
ECHO.
GOTO:EOF


:show
cd %DOS9_PATH%/share/doc/ansi

if "%~1" equ "" (
	
	if exist commands.ansi (
		
		type commands.ansi
		exit /b 0
	) else (
echo Unable to find default manual page. Is the man page database really built ?
echo If not, try to compile it using the /b switch.
		exit /b 1
	)
)

SET i=0

FOR /F "tokens=*" %%A IN ('dir /b /s *.ansi| find /i "%~1" ') DO (
	
	SET /a:I i+=1
	SET HlpPath[!i!]=%%A
 	SET HlpPath[!i!].name=%%~nA
	
)

if %i%==1 (

	more %HlpPath[1]%
	EXIT

) else if %i%==0 (
	
	ECHO Error: no manual page available for "%~1"
	ECHO.
	exit /b 1

) else (

	ECHO Looking for manual page "%~1"
	ECHO.
	ECHO There are many pages that match the name you gave.
	ECHO Choose the one you want to browse on the following menu:
	ECHO.
	
	FOR /L %%A in (1,1,%i%) do (
		ECHO 	%%A.	!HlpPath[%%A].name!
	)
	
	ECHO.
	SET /P choice=Your choice (either 'q' to quit or a number to select the page^) 
)

IF not !choice!==q (

	::cls
	more !HlpPath[%choice%]!

)
