@echo off
if %PBAT_OS%==WINDOWS (
set PATH=../dump;!PATH!
) else (
set PATH=!PATH!:../dump
)
%1 > %2
