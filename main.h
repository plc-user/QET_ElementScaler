/*
 * Copyright (c) 2022-2024 plc-user
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

#ifndef MAIN_H
#define MAIN_H

#include <iostream>     // for IO-Operations
#include <fstream>      // for file-Operations
#include <sstream>      // for String-Streams
#include <cstdint>      // int8_t, ...
#include <string>       // we handle strings here
#include <getopt.h>     // for Commandline-Parameters
#include <filesystem>   // for exe-filename
#include <regex>        // for "double"-Check
#include <list>         // for list of UUIDs

#define _DEBUG_ 0

// ============================================================================
// global variables
// ============================================================================

const std::string sVersion = "v0.5.0beta15";

// the element-file to process:
static std::string ElementFile       = "";
static std::string SVGFile           = "SampleFile.svg";
static std::string ElementFileScaled = "SampleFile.SCALED.elmt";

// we need a "Pugi-Node":
static pugi::xml_node node;

// some Bits for control
static bool xPrintHelp          = false;
static bool xReadFromStdIn      = false;
static bool xPrintToStdOut      = false;
static bool xCreateSVG          = false;
static bool xCreateELMT         = true;
static bool xStopWithError      = false;
static bool xScaleElement       = true;
static bool xRemoveAllTerminals = false;
static bool xOverwriteOriginal  = false;
static bool xFlipHor            = false;
static bool xFlipVert           = false;
static bool xRotate90           = false;

// to find out, if we need to renew UUIDs for "dynamic_text" or "terminal":
static std::list <std::string> lsUUIDsDynTexts;
static std::list <std::string> lsUUIDsTerminals;
static bool xTerminalsUUIDsUnique = true;
static bool xDynTextsUUIDsUnique = true;

// max. Number of decimals:
static size_t decimals = 2;    // number of decimals for floating-point values
static double MinLineLength = 0.01;  //

static double scaleX = 1.0;
static double scaleY = 1.0;

//
// --- function-prototypes -----------------------------------------------------
//
int parseCommandline(int argc, char *argv[]);
void PrintHelp(const std::string &s, const std::string &v);
bool CheckUUIDs(void);
void ProcessElement(pugi::xml_node);
std::string ToSVG(pugi::xml_node node);
std::string CheckForDoubleString(std::string &sArg);



// the possible Commandlineparameters:
static struct option long_options[]={
    {"file",required_argument,nullptr,'f'},
    {"help",no_argument,nullptr,'h'},
    {"stdin",no_argument,nullptr,'i'},
    {"stdout",no_argument,nullptr,'o'},
    {"factor",required_argument,nullptr,'F'},
    {"factorx",required_argument,nullptr,'x'},
    {"factory",required_argument,nullptr,'y'},
    {"decimals",required_argument,nullptr,'d'},
    {"RemoveAllTerminals",no_argument,nullptr,1000}, // "long-opt" only!!!
    {"FlipHorizontal",no_argument,nullptr,1001}, // "long-opt" only!!!
    {"FlipVertical",no_argument,nullptr,1002},   // "long-opt" only!!!
    {"OverwriteOriginal",no_argument,nullptr,1003},  // "long-opt" only!!!
    {"toSVG",no_argument,nullptr,1004},  // "long-opt" only!!!
    {"Rot90",no_argument,nullptr,1005},  // "long-opt" only!!!
    {0,0,0,0}
  };





/******************************************************************************/
int parseCommandline(int argc, char *argv[]) {
    int c;
    int option_index = 12345;
    std::string sTmp = "";
    while ((c=getopt_long(argc,argv,"f:hioF:x:y:d:",long_options,&option_index))!=-1) {
        if (_DEBUG_) std::cerr << "long-opt-name: " << long_options[option_index].name << std::endl;
        switch(c) {
            case 0:
                if (long_options[option_index].flag != 0){
                    if (_DEBUG_)
                        std::cerr << "(long_options[option_index].flag != 0)\n";
                    break;
                }
            case 1000:
                if (std::string(long_options[option_index].name) == "RemoveAllTerminals"){
                    if (_DEBUG_)
                        std::cerr << "Remove all terminals from Element!\n";
                    xRemoveAllTerminals = true;
                }
                break;
            case 1001:
                if (std::string(long_options[option_index].name) == "FlipHorizontal"){
                    if (_DEBUG_)
                        std::cerr << "Flip Element horizontally!\n";
                    xFlipHor = true;
                }
                break;
            case 1002:
                if (std::string(long_options[option_index].name) == "FlipVertical"){
                    if (_DEBUG_)
                        std::cerr << "Flip Element vertically!\n";
                    xFlipVert = true;
                }
                break;
            case 1003:
                if (std::string(long_options[option_index].name) == "OverwriteOriginal"){
                    if (_DEBUG_)
                        std::cerr << "Overwrite original file with scaled data!\n";
                    xOverwriteOriginal = true;
                }
                break;
            case 1004:
                if (std::string(long_options[option_index].name) == "toSVG"){
                    if (_DEBUG_)
                        std::cerr << "create SVG-data from Element-File\n";
                    xCreateSVG  = true;
                    xCreateELMT = false; // entoder-weder ... Beides gibt's nicht!
                }
                break;
            case 1005:
                if (std::string(long_options[option_index].name) == "Rot90"){
                    if (_DEBUG_)
                        std::cerr << "rotate element clockwise by 90 degree\n";
                    xRotate90 = true;
                }
                break;
            case 'd':
                sTmp = std::string(optarg);
                sTmp = CheckForDoubleString(sTmp);
                if ((sTmp == "WontWork") || (stoi(std::string(optarg)) < 0)) {
                    std::cerr << "could not convert \"" << optarg << "\" to positive number!" << std::endl;
                    xStopWithError = true;
                } else {
                    if (_DEBUG_)
                        std::cerr << "set number of decimals to " << optarg << "\n";
                    decimals = stoi(std::string(optarg));
                    MinLineLength = 1;
                    for (uint8_t i=0; i < decimals; i++) { MinLineLength /= 10.0; }
                }
                break;
            case 'i':
                if (_DEBUG_)
                    std::cerr << "Read from STDIN\n";
                xReadFromStdIn = true;
                break;
            case 'o':
                if (_DEBUG_)
                    std::cerr << "Output to STDOUT\n";
                xPrintToStdOut = true;
                break;
            case 'h':
                if (_DEBUG_)
                    std::cerr << "Help wanted...\n";
                xPrintHelp = true;
                break;
            case 'f':
                if (_DEBUG_)
                    std::cerr << "use filename \"" << optarg << "\"\n";
                ElementFile = std::string(optarg);
                break;
            case 'F':
                xScaleElement = true;
                sTmp = std::string(optarg);
                sTmp = CheckForDoubleString(sTmp);
                if (sTmp == "WontWork"){
                    std::cerr << "could not convert \"" << optarg << "\" to float!" << std::endl;
                    xStopWithError = true;
                } else {
                    if (_DEBUG_) std::cerr << "Factor: " << sTmp << std::endl;
                    scaleX = stod(sTmp);
                    scaleY = scaleX;
                    if (scaleX < 0.01) {
                        std::cerr << "scaling-factor negative or too small: " << sTmp << std::endl;
                        xStopWithError = true;
                    }
                }
                break;
            case 'x':
                xScaleElement = true;
                sTmp = std::string(optarg);
                sTmp = CheckForDoubleString(sTmp);
                if (sTmp == "WontWork"){
                    std::cerr << "could not convert \"" << optarg << "\" to float!" << std::endl;
                    xStopWithError = true;
                } else {
                    if (_DEBUG_) std::cerr << "FactorX: " << sTmp << std::endl;
                    scaleX = stod(sTmp);
                    if (scaleX < 0.01) {
                        std::cerr << "scaling-factor negative or too small: " << sTmp << std::endl;
                        xStopWithError = true;
                    }
                }
                break;
            case 'y':
                xScaleElement = true;
                sTmp = std::string(optarg);
                sTmp = CheckForDoubleString(sTmp);
                if (sTmp == "WontWork"){
                    std::cerr << "could not convert \"" << optarg << "\" to float!" << std::endl;
                    xStopWithError = true;
                } else {
                    if (_DEBUG_) std::cerr << "FactorY: " << sTmp << std::endl;
                    scaleY = stod(sTmp);
                    if (scaleY < 0.01) {
                        std::cerr << "scaling-factor negative or too small: " << sTmp << std::endl;
                        xStopWithError = true;
                    }
                }
                break;
            case '?':
                std::cerr << " * * * there were non-handled option(s)!"<< std::endl;
                xStopWithError = true;
                break;
        }
    }

    if (_DEBUG_) std::cerr << "optind=" << optind << "; argc="<<argc << std::endl;
    // are there "non-option-Arguments"?
    int iDiff = argc - optind;
    if (optind < argc) {
        if (_DEBUG_) std::cerr << "non-option ARGV-elements:" << std::endl;
        while (optind < argc) {
            if (_DEBUG_) std::cerr << optind << ": " << argv[optind] << std::endl;
            optind++;
        }
        if (_DEBUG_) std::cerr << std::endl;
    }
    // return number of non-option-arguments:
    return iDiff;
}
/******************************************************************************/


/******************************************************************************/
void PrintHelp(const std::string &s, const std::string &v){
    std::stringstream ss;
    ss << std::filesystem::path(s).filename();
    std::string sExeName = ss.str();
    std::cout << std::endl
    << sExeName << " version " << v << " needs arguments!" << std::endl
    << std::endl
    << "usage:" << std::endl
    << sExeName << "  [options]  FILENAME" << std::endl
    << std::endl
    << "   -i | --stdin     input-data is read from stdin, a given filename is  \n"
    << "                    ignored and scaled element will be written to stdout\n"
    << "   -o | --stdout    output will be written to stdout                    \n"
    << "   -F VALUE         or                                                  \n"
    << "   --factor VALUE   factor for both directions (x, y, rx, height, ...)  \n"
    << "   -x VALUE         or                                                  \n"
    << "   --factorx VALUE  factor for x-values (x, rx, width, ...)             \n"
    << "   -y VALUE         or                                                  \n"
    << "   --factory VALUE  factor for y-values (y, ry, height, ...)            \n"
    << "   -f FILENAME      or                                                  \n"
    << "   --file FILENAME  the file that will be used                          \n"
    << "   -d VALUE         or                                                  \n"
    << "   --decimals VALUE number of decimals for float-values in output       \n"
    << "   -h | --help      show this help                                      \n"
    << std::endl
    << "  there are also some \"long-opt\"-only options:                        \n"
    << std::endl
    << "  \"--toSVG\"              creates data for a Scalable-Vector-Graphic   \n"
    << "                         instead of a QElectroTech-element              \n"
    << "  \"--RemoveAllTerminals\" removes all terminals from the element       \n"
    << "                         (useful for front-views or \"thumbnails\")     \n"
    << "  \"--FlipHorizontal\"     flips all graphical elements horizontally    \n"
    << "                         (useful during creation of elements)           \n"
    << "  \"--FlipVertical\"       flips all graphical elements vertically      \n"
    << "                         (useful during creation of elements)           \n"
    << "  \"--Rot90\"              rotate element clockwise by 90 degree        \n"
    << "                         (useful during creation of elements)           \n"
    << "  \"--OverwriteOriginal\"  the original file is replaced by scaled one  \n"
    << "                         (CAUTION: Be careful with this option!)        \n"
    << std::endl
    << "As always with free software: Use it at your own risk!                \n\n";
}
/******************************************************************************/




/******************************************************************************/
bool CheckUUIDs(void) {
    uint64_t u64DynTextsOrg = lsUUIDsDynTexts.size();
    lsUUIDsDynTexts.sort();
    lsUUIDsDynTexts.unique();
    xDynTextsUUIDsUnique = (lsUUIDsDynTexts.size() == u64DynTextsOrg);
    if (xDynTextsUUIDsUnique == false) {
        std::cerr << " * * UUIDs of dynamic_texts are not unique: Create new ones! * *\n";
    }
    uint64_t u64TerminalsOrg = lsUUIDsTerminals.size();
    lsUUIDsTerminals.sort();
    lsUUIDsTerminals.unique();
    xTerminalsUUIDsUnique = (lsUUIDsTerminals.size() == u64TerminalsOrg);
    if (xTerminalsUUIDsUnique == false) {
        std::cerr << " * * UUIDs of terminals are not unique: Create new ones! * *\n";
    }
    return (xDynTextsUUIDsUnique && xTerminalsUUIDsUnique);
}
/******************************************************************************/



/******************************************************************************/
void ProcessElement(pugi::xml_node doc) {
    RectMinMax ElmtMinMax; // for Re-Calc of DefinitionLine!

    // create or renew element's uuid:
    if (!(doc.child("definition").child("uuid"))) {
        if (_DEBUG_) std::cerr << "Erstelle Element-UUID!\n" ;
        doc.child("definition").prepend_child("uuid");
        doc.child("definition").child("uuid").append_attribute("uuid").set_value(("{" + CreateUUID(false) + "}").c_str());
    } else {
        if (_DEBUG_) std::cerr << "Aktualisiere vorhandene Element-UUID!\n" ;
        doc.child("definition").child("uuid").attribute("uuid").set_value(("{" + CreateUUID(false) + "}").c_str());
    }
    // wenn die Anschlüsse alle weg sollen...
    if (xRemoveAllTerminals==true) {
        if (_DEBUG_) std::cerr << "change \"link_type\" to \"thumbnail\"" << std::endl;
        doc.child("definition").attribute("link_type").set_value("thumbnail");
        while(doc.child("definition").child("description").remove_child("terminal"));
        }
    // in einer Schleife die Elemente bearbeiten:
    pugi::xml_node node = doc.child("definition").child("description").first_child();
    // ... in a loop all parts
    for (; node; node = node.next_sibling())
    {
        if ((std::string(node.name())) == "rect") {
            ElmtRect rect;
            rect.Clear();
            rect.ReadFromPugiNode(node);
            if (xFlipHor)  rect.Flip();
            if (xFlipVert) rect.Mirror();
            if (xRotate90) rect.Rot90();
            rect.Scale(scaleX, scaleY);
            rect.WriteToPugiNode(node, decimals);
            ElmtMinMax.addx(rect.GetX());
            ElmtMinMax.addx(rect.GetX()+rect.GetWidth());
            ElmtMinMax.addy(rect.GetY());
            ElmtMinMax.addy(rect.GetY()+rect.GetHeight());
        }
        if ((std::string(node.name())) == "arc") {
            ElmtArc arc;
            arc.Clear();
            arc.ReadFromPugiNode(node);
            if (xFlipHor)  arc.Flip();
            if (xFlipVert) arc.Mirror();
            if (xRotate90) arc.Rot90();
            arc.Scale(scaleX, scaleY);
            arc.Normalize();
            arc.WriteToPugiNode(node, decimals);
            ElmtMinMax.addx(arc.GetMinX());
            ElmtMinMax.addx(arc.GetMaxX());
            ElmtMinMax.addy(arc.GetMinY());
            ElmtMinMax.addy(arc.GetMaxY());
        }
        if (((std::string(node.name())) == "ellipse") || ((std::string(node.name())) == "circle")) {
            ElmtEllipse elli;
            elli.Clear();
            elli.ReadFromPugiNode(node);
            if (xFlipHor)  elli.Flip();
            if (xFlipVert) elli.Mirror();
            if (xRotate90) elli.Rot90();
            elli.Scale(scaleX, scaleY);
            elli.WriteToPugiNode(node, decimals);
            ElmtMinMax.addx(elli.GetX());
            ElmtMinMax.addx(elli.GetX()+elli.GetWidth());
            ElmtMinMax.addy(elli.GetY());
            ElmtMinMax.addy(elli.GetY()+elli.GetHeight());
        }
        if ((std::string(node.name())) == "text") {
            ElmtText text;
            text.ReadFromPugiNode(node);
            if (xFlipHor)  text.Flip();
            if (xFlipVert) text.Mirror();
            if (xRotate90) text.Rot90();
            text.Scale(scaleX, scaleY);
            text.WriteToPugiNode(node, decimals);
            if (!((text.GetText() == "") || (text.GetText() == "_"))) {
                ElmtMinMax.addx(text.GetX()-text.GetSize());
                ElmtMinMax.addx(text.GetX()+text.GetSize());
                ElmtMinMax.addy(text.GetY()-text.GetSize());
                ElmtMinMax.addy(text.GetY()+text.GetSize());
            }
        }
        if ((std::string(node.name())) == "terminal") {
            ElmtTerminal term;
            term.ReadFromPugiNode(node);
            if (xFlipHor)  term.Flip();
            if (xFlipVert) term.Mirror();
            if (xRotate90) term.Rot90();
            term.Scale(scaleX, scaleY);
            if (node.attribute("uuid")) {
                lsUUIDsTerminals.push_back(node.attribute("uuid").as_string());
            }
            term.WriteToPugiNode(node);
            ElmtMinMax.addx(term.GetX()-5);
            ElmtMinMax.addx(term.GetX()+5);
            ElmtMinMax.addy(term.GetY()+5);
            ElmtMinMax.addy(term.GetY()-5);
        }
        if ((std::string(node.name())) == "dynamic_text") {
            ElmtDynText dyntext;
            dyntext.ReadFromPugiNode(node);
            if (xFlipHor)  dyntext.Flip();
            if (xFlipVert) dyntext.Mirror();
            if (xRotate90) dyntext.Rot90();
            dyntext.Scale(scaleX, scaleY);
            if (node.attribute("uuid")) {
                lsUUIDsDynTexts.push_back(node.attribute("uuid").as_string());
            }
            dyntext.WriteToPugiNode(node, decimals);
            if (!((dyntext.GetText() == "") || (dyntext.GetText() == "_"))) {
                ElmtMinMax.addx(dyntext.GetX()-dyntext.GetSize());
                ElmtMinMax.addx(dyntext.GetX()+dyntext.GetSize());
                ElmtMinMax.addy(dyntext.GetY()-dyntext.GetSize());
                ElmtMinMax.addy(dyntext.GetY()+dyntext.GetSize());
            }
        }
        if ((std::string(node.name())) == "line") {
            ElmtLine line;
            if ((line.ReadFromPugiNode(node) == true) && (line.GetLength() >= MinLineLength)) {
                if (xFlipHor)  line.Flip();
                if (xFlipVert) line.Mirror();
                if (xRotate90) line.Rot90();
                line.Scale(scaleX, scaleY);
                line.WriteToPugiNode(node, decimals);
                ElmtMinMax.addx(line.GetMinX());
                ElmtMinMax.addx(line.GetMaxX());
                ElmtMinMax.addy(line.GetMinY());
                ElmtMinMax.addy(line.GetMaxY());
            } else {
                // Invalid line will be ignored and deleted later!
                node.set_name("LINE_NodeToDelete");
            }
        }
        if ((std::string(node.name())) == "polygon") {
            ElmtPolygon poly;
            if (poly.ReadFromPugiNode(node) == true) {
                if (xFlipHor)  poly.Flip();
                if (xFlipVert) poly.Mirror();
                if (xRotate90) poly.Rot90();
                poly.Scale(scaleX, scaleY);
                poly.WriteToPugiNode(node, decimals);
                ElmtMinMax.addx(poly.GetMinX());
                ElmtMinMax.addx(poly.GetMaxX());
                ElmtMinMax.addy(poly.GetMinY());
                ElmtMinMax.addy(poly.GetMaxY());
            } else {
                // Invalid polygon will be ignored and deleted later!
                node.set_name("POLYGON_NodeToDelete");
            }
        }
    }
    // Cleanup the QET-Element by removing invalid parts:
    while(doc.child("definition").child("description").remove_child("LINE_NodeToDelete"));
    while(doc.child("definition").child("description").remove_child("POLYGON_NodeToDelete"));
    // die UUIDs prüfen, ob sie denn wirklich "unique" sind:
    if (CheckUUIDs() == false) {
        // in einer Schleife die UUIDs der Elemente bearbeiten:
        node = doc.child("definition").child("description").first_child();
        for (; node; node = node.next_sibling())
        {
            if (((std::string(node.name())) == "terminal") && (xTerminalsUUIDsUnique == false)) {
                std::string uuid = "{" + CreateUUID(false) + "}";
                node.attribute("uuid").set_value(uuid.c_str());
            }
            if (((std::string(node.name())) == "dynamic_text") && (xDynTextsUUIDsUnique == false)) {
                std::string uuid = "{" + CreateUUID(false) + "}";
                node.attribute("uuid").set_value(uuid.c_str());
            }
        }
    }
    // die Listen der UUIDs werden nicht mehr benötigt: leeren!
    lsUUIDsDynTexts.clear();
    lsUUIDsTerminals.clear();
    // die definitionLine muss auf jeden Fall angepasst werden:
    DefinitionLine defline;
    defline.ReadFromPugiNode(doc.child("definition"));
    defline.ReCalc(ElmtMinMax);
    defline.WriteToPugiNode(doc.child("definition"));
}
/******************************************************************************/




/******************************************************************************/
std::string ToSVG(pugi::xml_node node) {
    std::string s = "";
    s += "<svg xmlns=\"http://www.w3.org/2000/svg\"\n";
    s += "     xmlns:xlink=\"http://www.w3.org/1999/xlink\"\n";
    node = node.child("definition");
    // in der DefinitionLine stehen die Abmessungen drin:
    DefinitionLine DefLine;
    DefLine.ReadFromPugiNode(node);
    // Daten für Abmessungen aus Definition-Line:
    s += "     width=\"" + std::to_string(DefLine.Getwidth());
    s += "\" height=\"" + std::to_string(DefLine.Getheight()) + "\">\n";
    // hier die Definition von "terminal", falls im Element vorhanden:
    if (node.child("description").child("terminal")) {
        s += "  <defs>\n";
        s += "    <g id=\"terminal\" stroke-width=\"1\" stroke-linecap=\"square\">\n";
        s += "      <line x1=\"0\" y1=\"0\" x2=\"0\" y2=\"4\" stroke=\"#0000FF\" />\n";
        s += "      <line x1=\"0\" y1=\"1\" x2=\"0\" y2=\"4\" stroke=\"#FF0000\" />\n";
        s += "    </g>\n";
        s += "  </defs>\n";
    }
    // dann weiter mit der Gruppierung der Elemente inklusive Verschiebung mit Daten aus Definition-Line:
    // da drin auch die allgemeingültige Anweisung für "stroke-linecap=(butt|round|square)" bei QET: square
    s += "  <g transform=\"translate(" + std::to_string(DefLine.Gethotspot_x());
    s += "," + std::to_string(DefLine.Gethotspot_y()) + ")\" stroke-linecap=\"square\">\n";
    // Hier nun die Schleife für die grafischen Elemente
    // edit the graphical elements of the QET-Element:
    node = node.child("description").first_child();
    // ... in a loop
    for (; node; node = node.next_sibling())
    {   //std::cout << ".";
        if ((std::string(node.name())) == "rect") {
            ElmtRect rect;
            rect.Clear();
            rect.ReadFromPugiNode(node);
            s += "    "; s += rect.AsSVGstring(decimals); s += "\n";
        }
        if ((std::string(node.name())) == "text") {
            ElmtText text;
            text.Clear();
            text.ReadFromPugiNode(node);
            s += "    "; s += text.AsSVGstring(decimals); s += "\n";
        }
        if ((std::string(node.name())) == "dynamic_text") {
            ElmtDynText dyntext;
            dyntext.Clear();
            dyntext.ReadFromPugiNode(node);
            s += "    "; s += dyntext.AsSVGstring(decimals); s += "\n";
        }
        if (((std::string(node.name())) == "ellipse") || ((std::string(node.name())) == "circle")) {
            ElmtEllipse elli;
            elli.Clear();
            elli.ReadFromPugiNode(node);
            s += "    "; s += elli.AsSVGstring(decimals); s += "\n";
        }
        if ((std::string(node.name())) == "terminal") {
            ElmtTerminal term;
            term.Clear();
            term.ReadFromPugiNode(node);
            s += "    "; s += term.AsSVGstring(decimals); s += "\n";
        }
        if ((std::string(node.name())) == "arc") {
            ElmtArc arc;
            arc.Clear();
            arc.ReadFromPugiNode(node);
            s += "    "; s += arc.AsSVGstring(decimals); s += "\n";
        }
        if ((std::string(node.name())) == "polygon") {
            ElmtPolygon pol;
            pol.Clear();
            if (pol.ReadFromPugiNode(node) == true) {
                s += "    "; s += pol.AsSVGstring(decimals); s += "\n";
            } else {
                s += "    INVALID Polygon in Element-File\n";
            }
        }
        if ((std::string(node.name())) == "line") {
            ElmtLine lin;
            lin.Clear();
            if (lin.ReadFromPugiNode(node) == true) {
                s += "    "; s += lin.AsSVGstring(decimals); s += "\n";
            } else {
                s += "    INVALID Line in Element-File\n";
            }
        }
    }
    // und nun noch die letzten schließenden Tags:
    s += "  </g>\n  Your Browser does not support inline SVG!\n</svg>";
    // FERTIG!
    return s;
}
/******************************************************************************/


/******************************************************************************/
std::string CheckForDoubleString(std::string &sArg){
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
        const std::regex doubleRegEx ( R"rgx([\-\+]?\d+[\.\,]*\d+)rgx" );
        const std::regex intRegEx    ( R"rgx([\-\+]?\d+)rgx" );
        std::smatch sm;
        if(regex_match(sArg, sm, intRegEx) || regex_match(sArg, sm, doubleRegEx))
        {   // valid "double"- or "int"-value
            if (_DEBUG_)
                std::cerr << "can be converted to float: " << sm.str(0) << std::endl;
            return sArg;
        } else {
            // string is NOT convertable to double
            if (_DEBUG_)
                std::cerr << "can not convert \"" << sArg << "\" to float!" << std::endl;
            return "WontWork";
        }
}
/******************************************************************************/



#endif  //#ifndef MAIN_H
