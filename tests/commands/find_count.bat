@echo off
echo alpha>find_count.txt
echo beta>>find_count.txt
echo alpha>>find_count.txt
find /c alpha find_count.txt
