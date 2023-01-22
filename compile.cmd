@echo off

rem create sub-directories:
md obj\inc\pugixml > nul 2> nul

rem delete old files:
del obj\inc\pugixml\*.o   > nul 2> nul
del obj\*.o               > nul 2> nul
del QET_ElementScaler.exe > nul 2> nul

rem compile with GCC in "path"
@echo on
g++.exe -Wall -O2 -std=c++17 -c inc\pugixml\pugixml.cpp -o obj\inc\pugixml\pugixml.o
g++.exe -Wall -O2 -std=c++17 -c main.cpp -o obj\main.o
g++.exe -o QET_ElementScaler.exe obj\inc\pugixml\pugixml.o obj\main.o -s

pause
