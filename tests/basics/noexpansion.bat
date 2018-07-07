@echo off
setlocal disabledelayedexpansion
set index=1
set tbl[1]=test
echo %index% !tbl[%index%]!