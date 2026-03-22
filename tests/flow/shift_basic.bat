@echo off
call :show first second third
goto end
:show
echo 1=%1 2=%2
shift
echo 1=%1 2=%2
exit /b
:end
