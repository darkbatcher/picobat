@echo off
set VAR=outer
setlocal
set VAR=inner
echo A=%VAR%
endlocal
echo B=%VAR%
