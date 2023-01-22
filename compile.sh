#!/bin/sh

# create sub-directories
mkdir -p obj/inc/pugixml

# delete old files:
rm obj/inc/pugixml/*.o  &> /dev/null
rm obj/*.o              &> /dev/null
rm QET_ElementScaler

# compile QET_ElementScaler
g++ -Wall -O2 -std=c++17 -c inc/pugixml/pugixml.cpp -o obj/inc/pugixml/pugixml.o
g++ -Wall -O2 -std=c++17 -c main.cpp -o obj/main.o
g++ -o  QET_ElementScaler obj/inc/pugixml/pugixml.o obj/main.o  -s
