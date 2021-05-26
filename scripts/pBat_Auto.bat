@(
:: Picobat global initialization script
:: copyright (c) 2010-2018 Romain GARBI
::
:: This is free software. You can redistribute it and/or modify under the terms
:: of the GNU General Public Licence V3
::
:: Note : This code is usually wrapped in a @() block to make it quiet and
:: quick. Nevertheless, you can still make it verbose using the ECHO command
IF %PBAT_OS%==WINDOWS (

	:: set special PBAT directories for commands
	SET PATH=!PBAT_PATH!/cmd;!PBAT_PATH!;!PATH!

	:: Define additional path for sub-programs
	SET PBAT_SHARE=!PBAT_PATH!/share
	SET PBAT_ETC=!PBAT_PATH!/etc

) else (

	:: well this is an unix system. Use the unix conventions
	:: to expand the path and provide a little compatibility
	:: over windows.
	SET PATH=!PATH!:!PBAT_PATH!/cmd:!PBAT_PATH!
    SET PATHEXT=.bat:.cmd

	:: set windows compatible variable
	SET SYSTEMROOT=/bin
	SET SYSTEMDRIVE=/
	
	
	SET PROGRAMFILES=!PBAT_PATH!/bin
	SET USERPROFILE=%HOME%
	id -un | SET /p USERNAME=
	uname -n | SET /p USERDOMAIN=
	
	:: Define additionnal paths for sub-programs
	SET PBAT_SHARE=!PBAT_PATH!/share
	SET PBAT_ETC=!PBAT_PATH!/etc

    SET PBAT_START_SCRIPT=!PBAT_PATH!/cmd/pbat_start
)

ALIAS /f help=hlp.bat
SET PROMPT=$x.a;PBAT$x..; $P$G

IF exist !PBAT_ETC!/PBAT_Auto.bat CALL !PBAT_ETC!/PBAT_Auto.bat
IF exist !USERPROFILE!/.PBAT/PBAT_Auto.bat CALL !USERPROFILE!/.PBAT/PBAT_Auto.bat

IF %PBAT_IS_SCRIPT%==false (
PECHO $x9E;  ______   _______  _______   _____                                          
ECHO  (  __  \ (  ___  ^)(  ____ \ / ___ \  PBAT [%PBAT_VERSION%] - Beta release            
ECHO  ^| (  \  ^)^| (   ^) ^|^| (    \/( (   ^) ^) Copyright (c^) 2010-2021                
ECHO  ^| ^|   ^) ^|^| ^|   ^| ^|^| (_____ ( (___^) ^|    Romain Garbi, Teddy Astie           
ECHO  ^| ^|   ^| ^|^| ^|   ^| ^|(_____  ^) \____  ^|                                        
ECHO  ^| ^|   ^) ^|^| ^|   ^| ^|      ^) ^|      ^) ^| This is free software, you can modify  
ECHO  ^| (__/  ^)^| (___^) ^|/\____^) ^|/\____^) ^) and/or redistribute it under the terms 
ECHO  (______/ (_______^)\_______^)\______/  of the GNU Genaral Public License V3   
PECHO                                                                              $x..;
)

:: At this state, PBAT will reset neither options or variables that have
:: been set, so that, it might be something good to reset them by yourself.
:: The following line is used to disable delayed expansion. Uncomment if
:: you want to disable delayed expansion by default.
::
:: SETLOCAL DisableDelayedExpansion
)
