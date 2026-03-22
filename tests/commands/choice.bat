@echo off
choice /c:ABC /n /t:1 /d:B
echo T1=%errorlevel%
choice /c:XYZ /n /m "Pick one" /t:1 /d:Z
echo T2=%errorlevel%
choice /c:Q /n /t:Q,1
echo T3=%errorlevel%
choice /c:AAB /n /t:1 /d:A
echo T4=%errorlevel%
choice /c:AB /d:A
echo T5=%errorlevel%
