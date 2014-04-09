@(
:: Dos9 initialization script
:: copyright (c) 2010-2014 Darkbatcher  
::
:: This is free software. You can redistribute it and/or modify under the terms
:: of the GNU General Public Licence V3

:: Note : This code is usually wrapped in a @() block to make it quiet and
:: quick Nevertheless, you can still make it verbose using the ECHO command
SETLOCAL EnableDelayedExpansion

IF %DOS9_OS%==WINDOWS (

	:: set special dos9 directories for commands
	SET PATH=!DOS9_PATH!/cmd;!DOS9_PATH!;!PATH!

	:: set home path for support of the gnu nano editor
	:: broken since nano is no more includedd and it can
	:: causes incompatibility with other *nix packages for
	:: windows
	:: SET HOME=%DOS9_PATH%/nano
	
    	:: Set default language in windows
	:: Usually, this line is not needed and language
	:: is automatically detected.
	
    	:: SET LANGUAGE=fr_FR

) else (

	:: well this is an unix system. Use the unix conventions
	:: to expand the path and provide a little compatibility 
	:: over windows.

	SET PATH=!PATH!:!DOS9_PATH!/cmd:!PATH!
	
	:: set windows compatible variable
	SET SYSTEMROOT=/bin
	SET PROGRAMFILES=/usr/bin
	SET SYSTEMDRIVE=/
	SET USERPROFILE=~

)

:: At this state, dos9 will reset neither options or variables that have
:: been set, so that, it might be something good to reset them by yourself.
:: The following line is used to disable delayed expansion. Uncomment if
:: you want to disable delayed expansion by default.
:: 
:: SETLOCAL DisableDelayedExpansion
)
