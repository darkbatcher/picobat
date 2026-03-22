@echo off
call :sub hello
echo done
goto end
:sub
echo sub %1
exit /b
:end
