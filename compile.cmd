@echo off

rem create sub-directories:
md obj             > nul 2> nul
md obj\inc         > nul 2> nul
md obj\inc\pugixml > nul 2> nul

rem delete old files:
del obj\inc\pugixml\*.o   > nul 2> nul
del obj\inc\*.o           > nul 2> nul
del obj\*.o               > nul 2> nul
del QET_ElementScaler.exe > nul 2> nul

rem compile with GCC in "path"
@echo on

g++.exe -Wall -std=c++17 -O2 -Iinclude -c inc/pugixml/pugixml.cpp -o obj/inc/pugixml/pugixml.o
g++.exe -Wall -std=c++17 -O2 -Iinclude -c inc/helpers.cpp         -o obj/inc/helpers.o
g++.exe -Wall -std=c++17 -O2 -Iinclude -c inc/elements.cpp        -o obj/inc/elements.o
g++.exe -Wall -std=c++17 -O2 -Iinclude -c main.cpp                -o obj/main.o


rem link it all:
g++.exe -o QET_ElementScaler obj/inc/pugixml/pugixml.o obj/inc/helpers.o obj/inc/elements.o obj/main.o -s


@echo off
rem delete object-files:
del obj\inc\pugixml\*.o   > nul 2> nul
del obj\inc\*.o           > nul 2> nul
del obj\*.o               > nul 2> nul

@echo.
pause
@echo on
