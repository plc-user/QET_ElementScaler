//
// Header-File for:
// QET_ElementScaler
// a commandline-tool to scale QElectroTech-Graphics
// https://QElectroTech.org
//
// more information: see "main.cpp"
//
// Created from September 2022 to January 2023
// Author: plc-user
// https://github.com/plc-user
//

/*
 * Copyright (c) 2022-2023 plc-user
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
#include <getopt.h>     // for Commandline-Parameters
#include <regex>        // for "double"-Check
#include <filesystem>   // for exe-filename
#include <string>       // for std::string - handling
#include <sstream>      // for Type-Convertion
#include <random>       // for random values of uuid


//
//--- definition of class "RectMinMax" -----------------------------------------
//
class RectMinMax {
   private:
      double xMin = 0.0;
      double xMax = 0.0;
      double yMin = 0.0;
      double yMax = 0.0;
   public:
      RectMinMax();                  // default-constructor
      RectMinMax(double, double);    // constructor with value
      RectMinMax(const RectMinMax&); // copy-constructor
      void addx(double);             // add a new x-value
      void addy(double);             // add a new y-value
      void add(double, double);      // add new values
      void clear(void);              // clear min/max and set to 0
      void clear(double, double);    // clear min/max and set new values
      void clear(const RectMinMax&); // clear with value
      double xmin(void);             // returns minimum x-value
      double xmax(void);             // returns maximum x-value
      double ymin(void);             // returns minimum y-value
      double ymax(void);             // returns maximum y-value
      double width(void);            // returns (xmax - xmin)
      double height(void);           // returns (ymax - ymin)
      double diagonal(void);         // returns length of diagonal
      double angle(void);            // returns angle of diagonal
      friend std::ostream& operator << (std::ostream&, const RectMinMax&); // Standardoutput-operator
};
//
//--- implementation of class "RectMinMax" -------------------------------------
//
RectMinMax::RectMinMax(){
    xMin = 0.0;
    xMax = 0.0;
    yMin = 0.0;
    yMax = 0.0;
}
RectMinMax::RectMinMax(double x, double y){
    xMin = x;
    xMax = x;
    yMin = y;
    yMax = y;
}
RectMinMax::RectMinMax(const RectMinMax& r){
    xMin = r.xMin;
    xMax = r.xMax;
    yMin = r.yMin;
    yMax = r.yMax;
}
void RectMinMax::addx(double x){
    if (x < xMin) { xMin = x; }
    if (x > xMax) { xMax = x; }
}
void RectMinMax::addy(double y){
    if (y < yMin) { yMin = y; }
    if (y > yMax) { yMax = y; }
}
void RectMinMax::add(double x, double y){
    if (x < xMin) { xMin = x; }
    if (x > xMax) { xMax = x; }
    if (y < yMin) { yMin = y; }
    if (y > yMax) { yMax = y; }
}
// reset all values to ...
void RectMinMax::clear(void){
    xMin = 0.0;
    xMax = 0.0;
    yMin = 0.0;
    yMax = 0.0;
}
void RectMinMax::clear(double x, double y){
    xMin = x;
    xMax = x;
    yMin = y;
    yMax = y;
}
void RectMinMax::clear(const RectMinMax& r){
    xMin = r.xMin;
    xMax = r.xMax;
    yMin = r.yMin;
    yMax = r.yMax;
}
// return values seperately
double RectMinMax::xmin(void){
    return xMin;
}
double RectMinMax::xmax(void){
    return xMax;
}
double RectMinMax::ymin(void){
    return yMin;
}
double RectMinMax::ymax(void){
    return yMax;
}
double RectMinMax::width(void){
    return (xMax - xMin);
}
double RectMinMax::height(void){
    return (yMax - yMin);
}
double RectMinMax::diagonal(void){
    double w = (xMax - xMin);
    double h = (yMax - yMin);
    return std::sqrt((w * w)+(h * h));
}
double RectMinMax::angle(void){
    double dx = (xMax - xMin);
    double dy = (yMax - yMin);
    const double pi = 3.14159265359;
    return (atan2(dy, dx) * 180 / pi);
}
// for outputting the whole bunch:
inline std::ostream& operator << (std::ostream& strm, const RectMinMax& r)
{
	strm << "x:(" << r.xMin << " ... " << r.xMax << ")"
         << " | "
         << "y:(" << r.yMin << " ... " << r.yMax << ")";
	return strm;
}
//
//--- END - implementation of class "RectMinMax" -------------------------------
//


//
// --- function-prototypes for UUID-calculation --------------------------------
//
unsigned int random_char(void);
std::string generate_hex(const unsigned int);
std::string CreateUUID(void);
std::string CreateUUID(bool);
//
// ############################################################
// ###              we build a (random?) UUID               ###
// ############################################################
//
unsigned int random_char(void) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}
//
std::string generate_hex(const unsigned int len) {
    std::stringstream ss;
    for (unsigned int i = 0; i < len; i++) {
        const auto rc = random_char();
        std::stringstream hexstream;
        hexstream << std::hex << rc;
        auto hexa = hexstream.str();
        ss << (hexa.length() < 2 ? '0' + hexa : hexa);
    }
    return ss.str();
}
//
std::string CreateUUID(void) {
    std::string uuid = "";
    uuid  =       generate_hex(4);
    uuid += '-' + generate_hex(2);
    uuid += '-' + generate_hex(2);
    uuid += '-' + generate_hex(2);
    uuid += '-' + generate_hex(6);
    return uuid;
}
//
std::string CreateUUID(bool UpCase){
    std::string uuid = CreateUUID();
    for (size_t i=0; i<uuid.length(); i++)
        if (UpCase == true)
            uuid[i] = toupper(uuid[i]);
        else
            uuid[i] = tolower(uuid[i]);
    return uuid;
}
//
// ############################################################
// ### END          we build a (random?) UUID               ###
// ############################################################
//


//
// "... and now to something completely different!"
// variables and functions for the actual tasks:
//


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


RectMinMax XYMinMax;  // to store the min-max-values of the element


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
    char do_decimal_point()   const { return cDecSep; }  // Decimal-Separator
};


// function prototypes:
string CheckForDoubleString(string &sArg);
string FormatValue(double &val, const size_t decimals);
int parseCommandline(int argc, char *argv[]);
void PrintHelp(const string &s, const string &v);
void NormalizeArcVals(double &start, double &angle);
int ScaleFontSize(string &sFont, double dFactor);
int ReCalcDefinition(pugi::xml_node &node);
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
void DetermineArcMinMax(pugi::xml_node &node);
void DetermineMinMax(pugi::xml_node &node);


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------


/******************************************************************************/
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
/******************************************************************************/


/******************************************************************************/
void NormalizeArcVals(double &start, double &angle){
    int istart = (int)(round(start)) % 360;
    int iangle = (int)(round(angle)) % 360;
    if (istart < 0) {
        // for example:
        // old values: start = -20; angle = 330;
        // new values: start = 340; angle = 330;
        istart = (istart + 360) % 360;
        iangle = iangle;
    }
    if (iangle < 0) {
        // for example:
        // old values: start = 350; angle = -330;
        // new values: start =  20; angle =  330;
        istart  = (istart + iangle + 360) % 360;
        iangle *= (-1);
    }
    start = istart;
    angle = iangle;
}
/******************************************************************************/


//------------------------------------------------------------------------------
//------------------------------------------------------------------------------

#endif  //#ifndef _QETscaler_MAIN_H
