@(
:: Dos9 global initialization script
:: copyright (c) 2010-2016 Romain GARBI
::
:: This is free software. You can redistribute it and/or modify under the terms
:: of the GNU General Public Licence V3
::
:: Note : This code is usually wrapped in a @() block to make it quiet and
:: quick. Nevertheless, you can still make it verbose using the ECHO command
IF %DOS9_OS%==WINDOWS (

	:: set special dos9 directories for commands
	SET PATH=!DOS9_PATH!/cmd;!DOS9_PATH!;!PATH!

	:: Define additionnal path for sub-programs
	SET DOS9_SHARE=!DOS9_PATH!/share
	SET DOS9_ETC=!DOS9_PATH!/etc

) else (

	:: well this is an unix system. Use the unix conventions
	:: to expand the path and provide a little compatibility
	:: over windows.
	SET PATH=!PATH!:!DOS9_PATH!/cmd:!DOS9_PATH!
    SET PATHEXT=.bat:.cmd

	:: set windows compatible variable
	SET SYSTEMROOT=/bin
	SET PROGRAMFILES=!DOS9_PATH!/bin
	SET SYSTEMDRIVE=!DOS9_PATH!/bin
	SET "USERPROFILE=%HOME%"

	:: Define additionnal paths for sub-programs
	SET DOS9_SHARE=!DOS9_PATH!/share
	SET DOS9_ETC=!DOS9_PATH!/etc

    SET DOS9_START_SCRIPT=!DOS9_PATH!/cmd/dos9_start
)

ALIAS /f help=hlp.bat

IF exist !DOS9_ETC!/Dos9_Auto.bat CALL !DOS9_ETC!/Dos9_Auto.bat
IF exist !USERPROFILE!/.dos9/Dos9_Auto.bat CALL !USERPROFILE!/.dos9/Dos9_Auto.bat

:: At this state, dos9 will reset neither options or variables that have
:: been set, so that, it might be something good to reset them by yourself.
:: The following line is used to disable delayed expansion. Uncomment if
:: you want to disable delayed expansion by default.
::
:: SETLOCAL DisableDelayedExpansion
)
