#!/bin/bash

# create sub-directories
mkdir -p obj/inc/pugixml

# delete old files:
rm obj/inc/pugixml/*.o  &> /dev/null
rm obj/inc/*.o          &> /dev/null
rm obj/*.o              &> /dev/null
rm QET_ElementScaler    &> /dev/null

echo ""
echo ""
echo ""
echo "--------------------------------------------------------------------------------"

CompileOptions="-Wall -std=c++17 -O2"

# compile
g++ $CompileOptions  -c inc/pugixml/pugixml.cpp -o obj/inc/pugixml/pugixml.o
g++ $CompileOptions  -c inc/helpers.cpp -o obj/inc/helpers.o
g++ $CompileOptions  -c inc/elements.cpp -o obj/inc/elements.o
g++ $CompileOptions  -c main.cpp -o obj/main.o

# link:
g++  -o QET_ElementScaler \
        obj/inc/pugixml/pugixml.o \
        obj/inc/helpers.o \
        obj/inc/elements.o \
        obj/main.o  \
     -s

# delete object-files again:
rm obj/inc/pugixml/*.o  &> /dev/null
rm obj/inc/*.o          &> /dev/null
rm obj/*.o              &> /dev/null

echo "--------------------------------------------------------------------------------"
echo ""
echo ""
echo ""
