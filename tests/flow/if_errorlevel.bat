@echo off
set ERRORLEVEL=0
if errorlevel 1 echo bad1
set ERRORLEVEL=3
if errorlevel 4 echo bad2
if errorlevel 3 echo ge3
if errorlevel 2 echo ge2
if errorlevel 1 echo ge1
