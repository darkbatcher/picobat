@echo off
if "%1"=="row" goto row
goto main

:main
call %0 row 1
echo done
goto end

:row
echo row %2
goto end

:end
