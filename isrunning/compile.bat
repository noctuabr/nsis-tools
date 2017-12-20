@echo off
clang -Os -m32 src\isrunning.cpp -o isrunning.exe
strip isrunning.exe
