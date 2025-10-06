@echo off
C:\msys64\mingw64\bin\g++ main.cpp -std=c++17 -IC:/msys64/mingw64/include -LC:/msys64/mingw64/lib -o main.exe -lsfml-graphics -lsfml-window -lsfml-system
main.exe
pause
