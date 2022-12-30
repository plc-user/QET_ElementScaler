//
// Header-File for:
// QET_ElementScaler
// a commandline-tool to scale QElectroTech-Graphics
// https://QElectroTech.org
//
// more information: see "main.cpp"
//
// Created from September to December 2022
// Author: plc-user
// https://github.com/plc-user
//

/*
 * Copyright (c) 2022 plc-user
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef _QETscaler_MAIN_H
#define _QETscaler_MAIN_H

#include <iostream>     // I/O
#include <fstream>      // write stringstream to file
#include <cmath>        // for "round"
#include <getopt.h>     // for Commandline-Parameters
#include <locale>
#include <regex>        // for "double"-Check
#include <filesystem>   // for exe-filename
#include <sstream>      // für Typ-Konvertierung
#include <string>       // für das Ergebnis als std::string
#include <random>       // für Zufallszahlen der uuid

using namespace std;

const char cDecSep = '.';     // Decimal-Separator for values in output-file
const size_t decimals = 2;    // number of decimals for floating-point values
string sFileName = "NoFileNameSet.elmt";
string sFileScaled = "NoFileNameSet_scaled.elmt";
double scaleX = 1.0;
double scaleY = 1.0;

// some bits for control:
bool xStandardMode       = true;
bool xPrintHelp          = false;
bool xPrintToStdOut      = false;
bool xReadFromStdIn      = false;
bool xRemoveAllTerminals = false;
bool xFlipHor            = false;
bool xFlipVert           = false;
bool xOverwriteOriginal  = false;
bool xStopWithError      = false;


struct XYMinMaxVals{
    double xmin = 0.0;
    double ymin = 0.0;
    double xmax = 0.0;
    double ymax = 0.0;
};


// the possible Commandlineparameters:
static struct option long_options[]={
    {"file",required_argument,nullptr,'f'},
    {"help",no_argument,nullptr,'h'},
    {"stdin",no_argument,nullptr,'i'},
    {"stdout",no_argument,nullptr,'o'},
    {"factorx",required_argument,nullptr,'x'},
    {"factory",required_argument,nullptr,'y'},
    {"RemoveAllTerminals",no_argument,nullptr,1000}, // "long-opt" only!!!
    {"FlipHorizontal",no_argument,nullptr,1001}, // "long-opt" only!!!
    {"FlipVertical",no_argument,nullptr,1002},   // "long-opt" only!!!
    {"OverwriteOriginal",no_argument,nullptr,1003},  // "long-opt" only!!!
    {0,0,0,0}
  };


// for setting a (new) Decimal-Separator
struct DecSep : std::numpunct<char> {
    char do_decimal_point()   const { return cDecSep; }  // Dezimal-Trenner
};


// function prototypes:
string CheckForDoubleString(string &sArg);
string FormatValue(double &val, const size_t decimals);
int parseCommandline(int argc, char *argv[]);
void PrintHelp(const string &s, const string &v);
int ScaleFontSize(string &sFont, double dFactor);
int ReCalcDefinition(pugi::xml_node &node, XYMinMaxVals MiMaVals);
int ScaleElement(pugi::xml_node &node);
int ScalePoints(pugi::xml_node &node);
// Mirror basic shapes on the coordinate axes
int PolyLineFlipHor(pugi::xml_node &node);
int PolyLineFlipVert(pugi::xml_node &node);
int RectEllipseFlipVert(pugi::xml_node &node);
int RectEllipseFlipHor(pugi::xml_node &node);
int CircleFlipVert(pugi::xml_node &node);
int CircleFlipHor(pugi::xml_node &node);
int TerminalFlipVert(pugi::xml_node &node);
int TerminalFlipHor(pugi::xml_node &node);
int TextFlipVert(pugi::xml_node &node);
int TextFlipHor(pugi::xml_node &node);
int ArcFlipVert(pugi::xml_node &node);
int ArcFlipHor(pugi::xml_node &node);
int InputFlipVert(pugi::xml_node &node);
int InputFlipHor(pugi::xml_node &node);


unsigned int random_char();
string generate_hex(const unsigned int len);
string CreateUUID(void);
string CreateUUID(bool UpCase);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


/*****************************************************************************/
void PrintHelp(const string &s, const string &v){
    stringstream ss;
    ss << filesystem::path(s).filename();
    string sExeName = ss.str();
    string smilie = u8"\U0001F609";
    cout << endl
    << sExeName << " version " << v << " needs some arguments!" << endl
    << endl
    << "usage for simple mode:" << endl
    << sExeName << "  [options]  FILENAME  ScalingFactor" << endl
    << endl
    << "In extended mode the scaling-factors for X and Y may differ and it is" << endl
    << "allowed to specify only one direction for scaling. In each case the" << endl
    << "Font-Sizes and Circle-Diameters are scaled by the smaller value." << endl
    << endl
    << "usage for extended mode:" << endl
    << sExeName << " [options] [-x FactorX] [-y FactorY] -f FILENAME" << endl
    << endl << endl
    << "available options:" << endl
    << " -i | --stdin     input-data is read from stdin, a given filename is " << endl
    << "                  ignored and scaled element will be written to stdout" << endl
    << " -o | --stdout    output will be written to stdout" << endl
    << " -x VALUE         or" << endl
    << " --factorx VALUE  factor for x-values (x, rx, width, ...)" << endl
    << " -y VALUE         or" << endl
    << " --factory VALUE  factor for y-values (y, ry, height, ...)" << endl
    << " -f FILENAME      or " << endl
    << " --file FILENAME  the file that will be used" << endl
    << " -h | --help      show this help" << endl
    << endl << endl
    << "there are also some \"long-opt\"-only options:" << endl
    << "\"--RemoveAllTerminals\" removes all terminals from the element" << endl
    << "                       (useful for front-views or \"thumbnails\")" << endl
    << "\"--FlipHorizontal\"     flips all graphical elements horizontally" << endl
    << "                       (useful during creation of elements)" << endl
    << "\"--FlipVertical\"       flips all graphical elements vertically" << endl
    << "                       (useful during creation of elements)" << endl
    << "\"--OverwriteOriginal\"  the original file is replaced by scaled one" << endl
    << "                       (CAUTION: Be careful with this option!)" << endl
    << endl
    << "without parameters or with \"-h\" or \"--help\" this help is displayed" << endl
    << endl << endl
    << "The most simple call:" << endl
    << sExeName << " -f FILENAME" << endl
    << "A new file is created with all values rounded to two decimal places" << endl
    << "and all font sizes are set to integers - some kind of cleanup... " << endl
    << endl
    << "As always with free software: Use it at your own risk! " << smilie << endl
    << endl;
}
/*****************************************************************************/



// ############################################################
// ###          wir bauen eine (zufällige?) UUID            ###
// ############################################################
//
unsigned int random_char() {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}
//
string generate_hex(const unsigned int len) {
    stringstream ss;
    for (unsigned int i = 0; i < len; i++) {
        const auto rc = random_char();
        stringstream hexstream;
        hexstream << hex << rc;
        auto hex = hexstream.str();
        ss << (hex.length() < 2 ? '0' + hex : hex);
    }
    return ss.str();
}
//
string CreateUUID(void) {
    string sRet = "";
    sRet  =       generate_hex(4);
    sRet += '-' + generate_hex(2);
    sRet += '-' + generate_hex(2);
    sRet += '-' + generate_hex(2);
    sRet += '-' + generate_hex(6);
    return sRet;
}
//
string CreateUUID(bool UpCase){
    string uuid = CreateUUID();
    for (size_t i=0; i<uuid.length(); i++)
        if (UpCase == true)
            uuid[i] = toupper(uuid[i]);
        else
            uuid[i] = tolower(uuid[i]);
    return uuid;
}
// ############################################################
// ### ENDE       wir bauen eine (zufällige?) UUID          ###
// ############################################################
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif  //#ifndef _QETscaler_MAIN_H
