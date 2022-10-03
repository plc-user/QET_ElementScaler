rem compile with GCC in "path"

g++.exe -Wall -O2 -std=c++17 -c inc\pugixml\pugixml.cpp -o obj\inc\pugixml\pugixml.o
g++.exe -Wall -O2 -std=c++17 -c main.cpp -o obj\main.o
g++.exe -o QET_ElementScaler.exe obj\inc\pugixml\pugixml.o obj\main.o -s

pause
