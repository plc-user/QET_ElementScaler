//
// QET_ElementScaler is a commandline-tool to scale
// QElectroTech-Graphics with constant factor(s).
//
// compiles with (at least) C++17 enabled on
// - Debian/GNU Linux (unstable)
// - ReactOS (0.4.15-dev-5517)
// - macOS Monterey 12.6 (darwin21.6.0)
//
// to compile the code with GCC on Linux use these commands:
// g++ -Wall -O2 -std=c++17 -c inc/pugixml/pugixml.cpp -o obj/inc/pugixml/pugixml.o
// g++ -Wall -O2 -std=c++17 -c main.cpp -o obj/main.o
// g++ -o  QET_ElementScaler obj/inc/pugixml/pugixml.o obj/main.o  -s
//
// clang works as well:
// clang++ -Weverything -O2 -std=c++17 -c inc/pugixml/pugixml.cpp -o obj/inc/pugixml/pugixml.o
// clang++ -Weverything -O2 -std=c++17 -c main.cpp -o obj/main.o
// clang++ -o  QET_ElementScaler obj/inc/pugixml/pugixml.o obj/main.o  -s
//
// these commands work (TDM-GCC 10.3.0) on ReactOS to compile:
// g++.exe -Wall -O2 -std=c++17 -c inc\pugixml\pugixml.cpp -o obj\inc\pugixml\pugixml.o
// g++.exe -Wall -O2 -std=c++17 -c main.cpp -o obj\main.o
// g++.exe -o QET_ElementScaler.exe obj\inc\pugixml\pugixml.o obj\main.o -s
//
// compile for macOS (clang 14.0.0 / Monterey 12.6 / x86_64-apple-darwin21.6.0):
// g++ -Wall -O2 -std=c++17 -c inc/pugixml/pugixml.cpp -o obj/inc/pugixml/pugixml.o
// g++ -Wall -O2 -std=c++17 -c main.cpp -o obj/main.o
// g++ -o  QET_ElementScaler obj/inc/pugixml/pugixml.o obj/main.o
//
//
// usage:
// QET_ElementScaler [options] <file> <scaling-factor>
//
// OR
// QET_ElementScaler [options] [-x FactorForX] [-y FactorForY] -f FILENAME
//
//
// Result is a new file "FILENAME.SCALED.elmt" or output on stdout
// in both cases without the XML declaration-line
//
// Change(s) for 0.4beta9
// - added mathmatically correct calculation for Min-Max-Values of ARC
//
// Change(s) for 0.4beta8
// - graphical element "arc": normalize "start" and "angle" to positive values
// - internal: replaced struct for element's min-max-values by own class
// - internal: renamed variables, translated comments, sorted headers
//
// Change(s) for 0.4beta7
// - recalculate "hotspot_x", "hotspot_y", "width" and "height" of definition-line
//
// Change(s) for 0.4beta6
// - prevent warning on macOS
//
// Change(s) for 0.4beta5
// - The LongOpt-only Flip-Options are renamed to "FlipHorizontal" and "FlipVertical".
//   They now flip the complete Element at Coordinate-Axes.
//   ToDo: The Positions for "text" and "dynamic_text" have to be calculated
//         with Text-Width taken into account.
//
// Change(s) for 0.4beta4
// - updated PugiXML to version 1.13
//
// Change(s) for 0.4beta3
// - added values "start" and "angle" of "arc" for rounding to ... decimals
//
// Change(s) for 0.4beta2
// - terminals always placed on an integer-position: no decimals!
// - when removing all terminals, the "link_type" is set to "thumbnail"
//
// Change(s): 01.10.2022
// - ported all functionalities to C++
// - added possibility ro remove terminals
// - added possibility to flip all Lines and Polygons
// - added possibility to replace file with scaled one
//
// Created from September 2022 to January 2023
// Author: plc-user
// https://github.com/plc-user
//
// inspired by a code-fragment from a "pugixml"-sample
// https://github.com/zeux/pugixml
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


#include "inc/pugixml/pugixml.hpp"
#include "main.h"

const string sVersion = "0.4beta9";

const int _debug_ = 0;
const int _debug_points_ = 0;

/****************************************************************************/


/****************************************************************************/
/***                   some common Helper-Functions                       ***/
/****************************************************************************/


/****************************************************************************/
string CheckForDoubleString(string &sArg){
// regular expression for a valid floating point number with digit(s) before and after the separator:
// [\-\+]?\d+[\.\,]*\d+
// alternatively, integers are also valid:
// [\-\+]?\d+
// everything else shall be invalid!
// Source: https://regex101.com/
        for (size_t i=0; i<sArg.length(); i++) {
            if (sArg[i]== ',')  // allow comma as decimal separator
                sArg[i] = '.';
        }
        const regex doubleRegEx ( R"rgx([\-\+]?\d+[\.\,]*\d+)rgx" );
        const regex intRegEx    ( R"rgx([\-\+]?\d+)rgx" );
        smatch sm;
        if(regex_match(sArg, sm, intRegEx) || regex_match(sArg, sm, doubleRegEx))
        {   // valid "double"- or "int"-value
            if (_debug_)
                cerr << "can be converted to float: " << sm.str(0) << endl;
            return sArg;
        } else {
            // string is NOT convertable to double
            if (_debug_)
                cerr << "can not convert \"" << sArg << "\" to float!" << endl;
            return "WontWork";
        }
}
/****************************************************************************/


/****************************************************************************/
string FormatValue(double &val, const size_t dec){
    size_t i = 0; // index for loops
    double epsilon = 0.1; // too small values will be set to "0"
    for (i=0; i<dec; i++)
        epsilon = epsilon / 10.0;
    if (val < 0.0){    // for negative values
        if (val >= (epsilon * (-1.0)))
            val = 0.0;
    } else {           // for posivive values
        if (val <= epsilon)
            val = 0.0;
    }
    for (i=0; i<dec; i++)
        val = (val * 10.0);
    long int iVal = round(val);
    // we're done, if "0"
    if (iVal == 0)
        return "0";
    // additional rounding, if value is VERY close to the next integer:
    if (dec>1) {
        int Divider = 1;
        for (i=0; i<dec; i++) Divider *= 10;
        int Rest = iVal % Divider;
        if (((Rest > 0) && (Rest <  5)) || ((Rest < 0) && (Rest > -5))) {
            if (_debug_) cerr << "Divider: " << Divider << " - Rest: " << Rest
                              << " - subtract " << Rest << endl;
            iVal -= Rest;
        }
        if ((Rest > 0) && (Rest > (Divider - 5))) {
            if (_debug_) cerr << "Divider: " << Divider << " - Rest: " << Rest
                              << " - add " << (Divider - Rest) << endl;
            iVal += (Divider - Rest);
            }
        if ((Rest < 0) && (Rest < (-1)*(Divider - 5))) {
            if (_debug_) cerr << "Divider: " << Divider << " - Rest: " << Rest
                              << " - subtract " << (Divider + Rest) << endl;
            iVal -= (Divider + Rest);
            }
    }

    // prepare to return the new value as string
    double dVal = iVal;
    for (i=0; i<dec; i++)
        dVal = (dVal / 10.0);
    stringstream ss;
    ss.imbue(std::locale(std::cout.getloc(), new DecSep));
    ss << fixed << setprecision(2) << dVal;
    string s = ss.str();  // "s" is number always (!) with decimals
    // delete this range of characters in order from the end of string:
    const char chars[] = {'0', cDecSep};
    // use range for loop to print array elements
    for (char c : chars)
        while (s[s.length()-1] == c) {
            if (_debug_)
                cerr << "del \"" << c << "\"" << endl;
            s.erase(s.length()-1, 1);
        }

    return s;
}
/*****************************************************************************/


/*****************************************************************************/
int parseCommandline(int argc, char *argv[]) {
    int c;
    int option_index = 12345;
    string sTmp = "";
    while ((c=getopt_long(argc,argv,"f:hiox:y:",long_options,&option_index))!=-1) {
        if (_debug_) cerr << "long-opt-name: " << long_options[option_index].name << endl;
        switch(c) {
            case 0:
                if (long_options[option_index].flag != 0){
                    if (_debug_)
                        cerr << "(long_options[option_index].flag != 0)\n";
                    break;
                }
            case 1000:
                if (string(long_options[option_index].name) == "RemoveAllTerminals"){
                    if (_debug_)
                        cerr << "Remove all terminals from Element!\n";
                    xRemoveAllTerminals = true;
                }
                break;
            case 1001:
                if (string(long_options[option_index].name) == "FlipHorizontal"){
                    if (_debug_)
                        cerr << "Flip Element horizontally!\n";
                    xFlipHor = true;
                }
                break;
            case 1002:
                if (string(long_options[option_index].name) == "FlipVertical"){
                    if (_debug_)
                        cerr << "Flip Element vertically!\n";
                    xFlipVert = true;
                }
                break;
            case 1003:
                if (string(long_options[option_index].name) == "OverwriteOriginal"){
                    if (_debug_)
                        cerr << "Overwrite original file with scaled data!\n";
                    xOverwriteOriginal = true;
                }
                break;
            case 'i':
                if (_debug_)
                    cerr << "Read from STDIN\n";
                xReadFromStdIn = true;
                break;
            case 'o':
                if (_debug_)
                    cerr << "Output to STDOUT\n";
                xPrintToStdOut = true;
                break;
            case 'h':
                if (_debug_)
                    cerr << "Help wanted...\n";
                xPrintHelp = true;
                break;
            case 'f':
                if (_debug_)
                    cerr << "use filename \"" << optarg << "\"\n";
                sFileName = string(optarg);
                xStandardMode = false;
                break;
            case 'x':
                sTmp = string(optarg);
                sTmp = CheckForDoubleString(sTmp);
                if (sTmp == "WontWork"){
                    cerr << "could not convert \"" << optarg << "\" to float!" << endl;
                    xStopWithError = true;
                } else {
                    if (_debug_) cerr << "FactorX: " << sTmp << endl;
                    scaleX = stod(sTmp);
                    if (scaleX < 0.01) {
                        cerr << "scaling-factor negative or too small: " << sTmp << endl;
                        xStopWithError = true;
                    }
                }
                xStandardMode = false;
                break;
            case 'y':
                sTmp = string(optarg);
                sTmp = CheckForDoubleString(sTmp);
                if (sTmp == "WontWork"){
                    cerr << "could not convert \"" << optarg << "\" to float!" << endl;
                    xStopWithError = true;
                } else {
                    if (_debug_) cerr << "FactorY: " << sTmp << endl;
                    scaleY = stod(sTmp);
                    if (scaleY < 0.01) {
                        cerr << "scaling-factor negative or too small: " << sTmp << endl;
                        xStopWithError = true;
                    }
                }
                xStandardMode = false;
                break;
            case '?':
                cerr << " * * * there were non-handled option(s)!"<< endl;
                xStopWithError = true;
                break;
        }
    }

    if (_debug_) cerr << "optind=" << optind << "; argc="<<argc << endl;
    // are there "non-option-Arguments"?
    int iDiff = argc - optind;
    if (optind < argc) {
        if (_debug_) cerr << "non-option ARGV-elements:" << endl;
        while (optind < argc) {
            if (_debug_) cerr << optind << ": " << argv[optind] << endl;
            optind++;
        }
        if (_debug_) cerr << endl;
    }
    // return number of non-option-arguments:
    return iDiff;
}
/*****************************************************************************/


/*****************************************************************************/
int ScaleFontSize(string &sFont, double dFactor){
    // split sFont at ","
    // the second date is the font-size -> multiply with scaleX,
    // then first round and keep greater than "0", after that write back
    stringstream ss (sFont);
    int i = 0;
    int iSize = 0;
    if (_debug_) cerr << "font (org): " << sFont << endl;
    sFont.clear();
    while( ss.good() )
    {
        i++;
        string substr;
        getline( ss, substr, ',' );
        if (i==2){
            iSize = round(dFactor * (stod(substr)));
            iSize = max(1, iSize);
            substr = to_string(iSize);
        }
        sFont = sFont + substr + ",";
    }
    if (sFont[sFont.length()-1]==',')
        sFont.erase(sFont.length()-1, 1);
    if (_debug_) cerr << "font (new): " << sFont << endl;
    return iSize;
}
/*****************************************************************************/


/****************************************************************************/
/***                now the XML-specific functions                        ***/
/****************************************************************************/


/*****************************************************************************/
int ReCalcDefinition(pugi::xml_node &node){

    int width  = round(XYMinMax.width());
    int height = round(XYMinMax.height());

    // calculation taken from QET-sources:
    int upwidth = ((width/10)*10)+10;
    if ((width%10) > 6) upwidth+=10;

    int upheight = ((height/10)*10)+10;
    if ((height%10) > 6) upheight+=10;

    int xmargin = upwidth - width;
    int ymargin = upheight - height;

    int hotspot_x = -(round(XYMinMax.xmin() - (xmargin/2)));
    int hotspot_y = -(round(XYMinMax.ymin() - (ymargin/2)));

    // write new values to Definition-Line:
    for (pugi::xml_attribute attr: node.attributes())
    {
        if ((xRemoveAllTerminals==true) && (string(attr.name())=="link_type")){
            if (_debug_) cerr << "change \"link_type\" to \"terminal\"" << endl;
            attr.set_value("thumbnail");
        }
        if (string(attr.name())=="hotspot_x") {
            attr.set_value(to_string(hotspot_x).c_str());
        }
        if (string(attr.name())=="hotspot_y") {
            attr.set_value(to_string(hotspot_y).c_str());
        }
        if (string(attr.name())=="width") {
            attr.set_value(to_string(upwidth).c_str());
        }
        if (string(attr.name())=="height") {
            attr.set_value(to_string(upheight).c_str());
        }
    }

    return 0;
}
/*****************************************************************************/
int ScaleElement(pugi::xml_node &node){
    // for most of the elements
    if (_debug_) cerr << "Scale values of: " << node.name() << endl;

    // diameter of circles and font-sizes are scaled by the smaller value
    double smallerScale = ((scaleX<scaleY)? (scaleX) : (scaleY));

    if (sizeof(node.attribute("circle").name()) != 0) {
        double diameter = node.attribute("diameter").as_double();
        diameter *= smallerScale;
        node.attribute("diameter") = FormatValue(diameter, decimals).c_str();
    }

    if (sizeof(node.attribute("text").name()) != 0) {
        int iFontSize = (node.attribute("size").as_int());
        if (iFontSize > 0){
            double dFontSize = double(iFontSize) * smallerScale;
            iFontSize = round(dFontSize);
            if (iFontSize < 1) iFontSize = 1;
            node.attribute("size") = (to_string(iFontSize)).c_str();
        }
    }

    if (sizeof(node.attribute("dynamic_text").name()) != 0) {
        string sFont = string(node.attribute("font").as_string());
        if (!(sFont == "")){
            ScaleFontSize(sFont, smallerScale);
            node.attribute("font") = sFont.c_str();
        }
    }

    if (sizeof(node.attribute("rx").name()) != 0) {
        double rx = node.attribute("rx").as_double();
        rx *= scaleX;
        node.attribute("rx") = FormatValue(rx, decimals).c_str();
    }

    if (sizeof(node.attribute("ry").name()) != 0) {
        double ry = node.attribute("ry").as_double();
        ry *= scaleY;
        node.attribute("ry") = FormatValue(ry, decimals).c_str();
    }

    if (sizeof(node.attribute("width").name()) != 0) {
        double width = node.attribute("width").as_double();
        width *= scaleX;
        node.attribute("width") = FormatValue(width, decimals).c_str();
    }

    if (sizeof(node.attribute("height").name()) != 0) {
        double height = node.attribute("height").as_double();
        height *= scaleY;
        node.attribute("height") = FormatValue(height, decimals).c_str();
    }

    double posX = node.attribute("x").as_double();
    posX *= scaleX;
    if (static_cast<string>(node.name())=="terminal") {
        // terminals without decimals:
        node.attribute("x") = FormatValue(posX, 0).c_str();
    } else {
        // other elements may have decimals:
        node.attribute("x") = FormatValue(posX, decimals).c_str();
    }

    double posY = node.attribute("y").as_double();
    posY *= scaleY;
    if (static_cast<string>(node.name())=="terminal") {
        // terminals without decimals:
        node.attribute("y") = FormatValue(posY, 0).c_str();
    } else {
        // other elements may have decimals:
        node.attribute("y") = FormatValue(posY, decimals).c_str();
    }

    return 0;
}
/*****************************************************************************/
int ScalePoints(pugi::xml_node &node){
    // for elements "line" and "polygon"
    if (_debug_) cerr << "Scale values of: " << node.name() << endl;
    double length;
    double posX;
    double posY;

    for (pugi::xml_attribute attr: node.attributes())
    {
        if (_debug_points_) cerr << " " << attr.as_double() << "=" << attr.value();
        if (attr.name()[0]=='x') {
            posX = attr.as_double() * scaleX;
            attr.set_value(FormatValue(posX, decimals).c_str());
        }
        if (attr.name()[0]=='y') {
            posY = attr.as_double() * scaleY;
            attr.set_value(FormatValue(posY, decimals).c_str());
        }
        if ((static_cast<string>(attr.name())=="length1") || (static_cast<string>(attr.name())=="length2")) {
            length = attr.as_double();
            attr.set_value(FormatValue(length, decimals).c_str());
        }
    }
    if (_debug_points_) cerr << endl;

    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
int PolyLineFlipHor(pugi::xml_node &node){
    // only for elements "line" and "polygon"
    if (_debug_) cerr << "Flip horizontal: " << node.name() << endl;
    double posX;
    // for points of "polygon" and "line" invert the sign:
    for (pugi::xml_attribute attr: node.attributes())
    {
        if (_debug_points_) cerr << " " << attr.as_double() << "=" << attr.value();
        if (attr.name()[0]=='x') {
            posX = attr.as_double() * (-1);
            attr.set_value(FormatValue(posX, decimals).c_str());
        }
    }

    if (_debug_points_) cerr << endl;

    return 0;
}
/*****************************************************************************/
int  PolyLineFlipVert(pugi::xml_node &node){
    // only for elements "line" and "polygon"
    if (_debug_) cerr << "Flip vertical: " << node.name() << endl;
    double posY;
    // for points of "polygon" and "line" invert the sign:
    for (pugi::xml_attribute attr: node.attributes())
    {
        if (_debug_points_) cerr << " " << attr.as_double() << "=" << attr.value();
        if (attr.name()[0]=='y') {
            posY = attr.as_double() * (-1);
            attr.set_value(FormatValue(posY, decimals).c_str());
        }
    }

    if (_debug_points_) cerr << endl;

    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
int  RectEllipseFlipVert(pugi::xml_node &node){
    if (_debug_) cerr << "Flip vertical: " << node.name() << endl;
    // calculate new Y-values and replace in element:
    double posY = (-1) * node.attribute("y").as_double() - node.attribute("height").as_double();
    node.attribute("y") = FormatValue(posY, decimals).c_str();
    return 0;
}
/*****************************************************************************/
int RectEllipseFlipHor(pugi::xml_node &node){
    if (_debug_) cerr << "Flip horizontal: " << node.name() << endl;
    // calculate new X-values and replace in element:
    double posX = (-1) * node.attribute("x").as_double() - node.attribute("width").as_double();
    node.attribute("x") = FormatValue(posX, decimals).c_str();
    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
int ArcFlipVert(pugi::xml_node &node){
    if (_debug_) cerr << "Flip vertical: " << node.name() << endl;
    // calculate new Y-values and replace in element:
    double posY = (-1) * node.attribute("y").as_double() - node.attribute("height").as_double();
    node.attribute("y") = FormatValue(posY, decimals).c_str();
    double Start = (-1) * node.attribute("start").as_double();
    double Angle = (-1) * node.attribute("angle").as_double();
    node.attribute("start") = FormatValue(Start, decimals).c_str();
    node.attribute("angle") = FormatValue(Angle, decimals).c_str();
    return 0;
}
/*****************************************************************************/
int ArcFlipHor(pugi::xml_node &node){
    if (_debug_) cerr << "Flip horizontal: " << node.name() << endl;
    // calculate new X-values and replace in element:
    double posX = (-1) * node.attribute("x").as_double() - node.attribute("width").as_double();
    node.attribute("x") = FormatValue(posX, decimals).c_str();
    double Start = 180 - node.attribute("start").as_double();
    double Angle = (-1) * node.attribute("angle").as_double();
    node.attribute("start") = FormatValue(Start, decimals).c_str();
    node.attribute("angle") = FormatValue(Angle, decimals).c_str();
    return 0;
}
/*****************************************************************************/
int NormalizeArc(pugi::xml_node &node){
    if (_debug_) cerr << "normalize angles of: " << node.name() << endl;
    double Start = node.attribute("start").as_double();
    double Angle = node.attribute("angle").as_double();
    NormalizeArcVals(Start, Angle);
    node.attribute("start") = FormatValue(Start, decimals).c_str();
    node.attribute("angle") = FormatValue(Angle, decimals).c_str();
    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
int CircleFlipVert(pugi::xml_node &node){
    if (_debug_) cerr << "Flip vertical: " << node.name() << endl;
    // calculate new Y-value and replace in element:
    double posY = (-1) * node.attribute("y").as_double() - node.attribute("diameter").as_double();
    node.attribute("y") = FormatValue(posY, decimals).c_str();
    return 0;
}
/*****************************************************************************/
int CircleFlipHor(pugi::xml_node &node){
    if (_debug_) cerr << "Flip horizontal: " << node.name() << endl;
    // calculate new X-value and replace in element:
    double posX = (-1) * node.attribute("x").as_double() - node.attribute("diameter").as_double();
    node.attribute("x") = FormatValue(posX, decimals).c_str();
    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
int TerminalFlipVert(pugi::xml_node &node){
    if (_debug_) cerr << "Flip vertical: " << node.name() << endl;
    // calculate new Y-value and replace in element:
    double posY = (-1) * node.attribute("y").as_double();
    node.attribute("y") = FormatValue(posY, decimals).c_str();
    // set new orientation for terminal:
    std::string sOrient = std::string(node.attribute("orientation").value());
    if (sOrient == "n") { node.attribute("orientation").set_value("s"); }
    if (sOrient == "s") { node.attribute("orientation").set_value("n"); }
    return 0;
}
/*****************************************************************************/
int TerminalFlipHor(pugi::xml_node &node){
    if (_debug_) cerr << "Flip horizontal: " << node.name() << endl;
    // calculate new X-value and replace in element:
    double posX = (-1) * node.attribute("x").as_double();
    node.attribute("x") = FormatValue(posX, decimals).c_str();
    // set new orientation for terminal:
    std::string sOrient = std::string(node.attribute("orientation").value());
    if (sOrient == "w") { node.attribute("orientation").set_value("e"); }
    if (sOrient == "e") { node.attribute("orientation").set_value("w"); }
    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
int TextFlipVert(pugi::xml_node &node){
    if (_debug_) cerr << "Flip vertical: " << node.name() << endl;
    std::string sFont = std::string(node.attribute("font").as_string());
    double FontSize;
    if (sFont == "") {
          FontSize = node.attribute("size").as_double();
          }
      else {
          FontSize = ScaleFontSize(sFont, 1.0);
          }
    // calculate new Y-value and replace in element:
    double posY = (-1) * node.attribute("y").as_double() + FontSize;
    node.attribute("y") = FormatValue(posY, decimals).c_str();
    return 0;
}
/*****************************************************************************/
int TextFlipHor(pugi::xml_node &node){
    if (_debug_) cerr << "Flip horizontal: " << node.name() << endl;
    // calculate new X-value and replace in element:
    double posX = (-1) * node.attribute("x").as_double();
    node.attribute("x") = FormatValue(posX, decimals).c_str();
    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
int InputFlipVert(pugi::xml_node &node){
    if (_debug_) cerr << "Flip vertical: " << node.name() << endl;
    // calculate new Y-value and replace in element:
    double posY = (-1) * node.attribute("y").as_double() - node.attribute("size").as_double();
    node.attribute("y") = FormatValue(posY, decimals).c_str();
    return 0;
}
/*****************************************************************************/
int InputFlipHor(pugi::xml_node &node){
    if (_debug_) cerr << "Flip horizontal: " << node.name() << endl;
    // calculate new X-value and replace in element:
    double posX = (-1) * node.attribute("x").as_double();
    node.attribute("x") = FormatValue(posX, decimals).c_str();
    return 0;
}
/*****************************************************************************/


/*****************************************************************************/
void DetermineArcMinMax(pugi::xml_node &node){
    // read values from XML:
    double PosX   = node.attribute("x").as_double();
    double PosY   = node.attribute("y").as_double();
    double Height = node.attribute("height").as_double();
    double Width  = node.attribute("width").as_double();
    int StartDeg  = round(node.attribute("start").as_double());
    int AngleDeg  = round(node.attribute("angle").as_double());
    const double pi = 3.14159265359;
    // calculate points on ellipse-circumference:
    // x = a ⋅ cos(t)
    // y = b ⋅ sin(t)
    // with t = 0 .. 2*pi = ([0 .. 360]/360)*2*pi
    for (int i = StartDeg; i <= (StartDeg + AngleDeg); i++){
        double x = PosX + (Width/2) * (1 + cos((i%360)/360.0*2*pi));
        double y = PosY + (Height/2) * (1 - sin((i%360)/360.0*2*pi));
        XYMinMax.add(x, y);
    }
    return;
}
/*****************************************************************************/

/*****************************************************************************/
void DetermineMinMax(pugi::xml_node &node){
    // no need to make a difference, what node it is: non-existing attributes return "0.0"
    XYMinMax.addx(node.attribute("x").as_double());
    XYMinMax.addx(node.attribute("x").as_double() + node.attribute("width").as_double());
    XYMinMax.addx(node.attribute("x").as_double() + node.attribute("diameter").as_double());
    XYMinMax.addy(node.attribute("y").as_double());
    XYMinMax.addy(node.attribute("y").as_double() + node.attribute("height").as_double());
    XYMinMax.addy(node.attribute("y").as_double() + node.attribute("diameter").as_double());
    // separate handling for lines and polygons:
    if (((string(node.name())) == "line")       ||
        ((string(node.name())) == "polygon"))     {
        for (pugi::xml_attribute attr: node.attributes())
        {
            if (attr.name()[0]=='x') {
                XYMinMax.addx(attr.as_double());
            }
            if (attr.name()[0]=='y') {
                XYMinMax.addy(attr.as_double());
            }
        }
    }
    return;
}
/*****************************************************************************/




/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/
int main(int argc, char *argv[]) {

    // we need a "Pugi-Node":
    pugi::xml_node node;


    // read and check the Commandline-Parameters
    int iRetVal = parseCommandline(argc, argv);
    if (_debug_) cerr << "iRetVal: " << iRetVal << endl;


    // who needs help, does not run the program yet:
    if ((xPrintHelp) || (argc<3)){
        PrintHelp(argv[0], sVersion);
        return 0;
    }


    if (xStopWithError == true){
        cerr << "check options and try again!" << endl;
        return -1;
    }


    // is the second-last "non-option" parameter a valid file?
    if ((xStandardMode == true) && (xReadFromStdIn == false) && (iRetVal == 2) && (filesystem::exists(argv[argc-2]))){
        sFileName = argv[argc-2];
        if (_debug_) cerr << sFileName << endl;
    }


    // is the last "non-option" parameter a floating point number?
    if ((xStandardMode == true) && (iRetVal >= 1)){
        string sTmp = (argv[argc-1]);
        sTmp = CheckForDoubleString(sTmp);
        if (sTmp == "WontWork"){
            cerr << "could not convert \""<<argv[argc-1]<<"\" to a float-number!" << endl;
            return -1;
        } else {
            if (_debug_) cerr << "scaling-factor: " << sTmp << endl;
            scaleX = stod(sTmp);
            scaleY = scaleX;
            if (scaleX < 0.01) {
                cerr << "scaling-factor negative or too small: " << sTmp << endl;
                return -1;
            }
        }
    }


    // build the filename for the scaled element:
    sFileScaled = sFileName;
    if (xOverwriteOriginal == true){
        cerr << "will overwrite original file!" << endl;
    } else {
        sFileScaled.insert(sFileScaled.length()-5, ".SCALED");
    }
    if (_debug_) cerr << sFileScaled << endl;


    /*************************************************************************/
    /***             now finally the XML elements are edited               ***/
    /*************************************************************************/


    // load Element from stdin or XML-File
    pugi::xml_document doc;
    pugi::xml_parse_result result;
    if (xReadFromStdIn){
        result = doc.load(cin);
    } else {
        result = doc.load_file(sFileName.c_str());
    }
    // check the result of "doc.load"-Function
    if (!result){
        cerr << "file \"" << sFileName << "\" could not be loaded: " << result.description() << endl;
        return -1;
    }


    // we re-new the elements UUID:
    node = doc.child("definition").child("uuid");
    for (pugi::xml_attribute attr: node.attributes())
    {
        if (string(attr.name())=="uuid") {
            if (_debug_) cerr << "create new uuid - old: " << attr.value() << endl;
            string sUUID = "{" + CreateUUID(false) + "}";
            if (_debug_) cerr << "             new uuid: " << sUUID << endl;
            attr.set_value(sUUID.c_str());
        }
    }


    // Remove all Terminals from QET-Element:
    if (xRemoveAllTerminals == true){
        node = doc.child("definition").child("description");
        while(node.remove_child("terminal"));
    }


    // edit the graphical elements of the QET-Element:
    node = doc.child("definition").child("description").first_child();

    for (; node; node = node.next_sibling())
    {
        if (((string(node.name())) == "line")     ||
            ((string(node.name())) == "polygon"))    {
            ScalePoints(node);
            if (xFlipHor==true) { PolyLineFlipHor(node); }
            if (xFlipVert==true) { PolyLineFlipVert(node); }
            DetermineMinMax(node);
        }
        if (((string(node.name())) == "ellipse")  ||
            ((string(node.name())) == "rect"))           {
            ScaleElement(node);
            if (xFlipHor==true) { RectEllipseFlipHor(node); }
            if (xFlipVert==true) { RectEllipseFlipVert(node); }
            DetermineMinMax(node);
        }
        if ((string(node.name())) == "circle") {
            ScaleElement(node);
            if (xFlipHor==true) { CircleFlipHor(node); }
            if (xFlipVert==true) { CircleFlipVert(node); }
            DetermineMinMax(node);
        }
        if ((string(node.name())) == "arc") {
            ScaleElement(node);
            if (xFlipHor==true) { ArcFlipHor(node); }
            if (xFlipVert==true) { ArcFlipVert(node); }
            NormalizeArc(node);
            DetermineArcMinMax(node);
        }
        if ((string(node.name())) == "terminal") {
            ScaleElement(node);
            if (xFlipHor==true) { TerminalFlipHor(node); }
            if (xFlipVert==true) { TerminalFlipVert(node); }
            DetermineMinMax(node);
        }
        if (((string(node.name())) == "dynamic_text") ||
            ((string(node.name())) == "text")) {
            ScaleElement(node);
            if (xFlipHor==true) { TextFlipHor(node); }
            if (xFlipVert==true) { TextFlipVert(node); }
            // Min-Max-Calculation not for text-nodes
        }
        if ((string(node.name())) == "input") {
            ScaleElement(node);
            if (xFlipHor==true) { InputFlipHor(node); }
            if (xFlipVert==true) { InputFlipVert(node); }
            // Min-Max-Calculation not for text-nodes
        }
    }

    if (_debug_ ){
        cerr << "XYMinMax: " << XYMinMax << endl
             << "Diagonal: " << XYMinMax.diagonal() << " Angle: " << XYMinMax.angle() << "°" << endl;
    }

    // calculate new values for Definition-Line:
    node = doc.child("definition");
    ReCalcDefinition(node);


    if (xPrintToStdOut==true){
        if (_debug_) cerr << "XML auf stdout ------------------------------------------------------" << endl;
        doc.save(cout, "    ", pugi::format_default | pugi::format_no_declaration);
        // no result from "doc.save" when sending XML to cout ...
        if (_debug_) cerr << "XML auf stdout ------------------------------------------------------" << endl;
        return 0;
    } else {
        // we try to save the new XML:
        if (doc.save_file(sFileScaled.c_str(), "    ", pugi::format_default | pugi::format_no_declaration) == true){
            if (_debug_) cerr << "file \"" << sFileScaled << "\" saved successfully!" << endl;
            return 0;
        } else {
            cerr << "file \"" << sFileScaled << "\" could not be saved!" << endl;
            return -1;
        }
    }
    return 0;
}
