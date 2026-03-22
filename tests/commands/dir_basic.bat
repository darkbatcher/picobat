@echo off
mkdir dir_basic_dir
echo x>dir_basic_dir/a.txt
echo y>dir_basic_dir/b.txt
cd dir_basic_dir
dir /b
