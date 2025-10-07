@echo off
C:/msys64/mingw64/bin/g++.exe -std=c++17 -g main.cpp ^
-I C:/msys64/mingw64/include ^
-L C:/msys64/mingw64/lib ^
-lsfml-graphics -lsfml-window -lsfml-system -lsfml-audio ^
-o main.exe

pause
