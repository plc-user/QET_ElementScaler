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

#include "helpers.h"
#include "elements.h"

#include <cmath>        // for sqrt, atan2, isnan(), ...
#include <iomanip>      // for IO-Operations
#include <string>       // for string-handling
#include <map>          // for Color-Map


//
//--- implementation of class "DefinitionLine" ---------------------------------
//
bool DefinitionLine::ReadFromPugiNode(pugi::xml_node node)
{
    version   = node.attribute("version").as_string();
    link_type = node.attribute("link_type").as_string();
    type      = node.attribute("type").as_string();
    width     = node.attribute("width").as_int();
    height    = node.attribute("height").as_int();
    hotspot_x = node.attribute("hotspot_x").as_int();
    hotspot_y = node.attribute("hotspot_y").as_int();
    return true;
}
// ---
void DefinitionLine::ReCalc(RectMinMax XYMinMax) {
// size and hotspots have to be re-calculated after scaling!
    int w = round(XYMinMax.width());
    int h = round(XYMinMax.height());

    // calculation taken from QET-sources:
    int upwidth = ((w/10)*10)+10;
    if ((w%10) > 6) upwidth+=10;
    int upheight = ((h/10)*10)+10;
    if ((h%10) > 6) upheight+=10;
    int xmargin = upwidth - w;
    int ymargin = upheight - h;

    // copy values to internal variables
    width = upwidth;
    height = upheight;
    hotspot_x = -(round(XYMinMax.xmin() - (xmargin/2)));
    hotspot_y = -(round(XYMinMax.ymin() - (ymargin/2)));
}
// ---
bool DefinitionLine::WriteToPugiNode(pugi::xml_node node)
{
    node.attribute("version").set_value(version.c_str());
    node.attribute("link_type").set_value(link_type.c_str());
    node.attribute("type").set_value(type.c_str());
    node.attribute("width").set_value(std::to_string(width).c_str());
    node.attribute("height").set_value(std::to_string(height).c_str());
    node.attribute("hotspot_x").set_value(std::to_string(hotspot_x).c_str());
    node.attribute("hotspot_y").set_value(std::to_string(hotspot_y).c_str());
    return true;
}
//
//--- END - implementation of class "DefinitionLine" ---------------------------
//



//
//--- implementation of class "BaseStyle" --------------------------------------
//
std::string BaseStyle::StyleAsSVGstring(const uint8_t decimals)
{
// in Element:   style = "line-style:normal;line-weight:normal;filling:none;color:black"
// SVG-Beispiel: style="fill:yellow;stroke:purple;stroke-width:2"
//               style="stroke:rgb(255,0,0);stroke-width:2"
//
    // wir teilen den Style-String zunächst in Teil-Strings:
    std::vector<std::string> vsStyleParts;
    std::string text = style;
    //std::cerr << text << std::endl;
    size_t pos;
    std::string delimiter = ";";
    while ((pos = text.find(delimiter)) != std::string::npos) {
        vsStyleParts.push_back(text.substr(0, pos));
        text.erase(0, pos + delimiter.length());
    }
    // der letzte teil muss auch mit:
    vsStyleParts.push_back(text);
    // wir teilen den style-string auf und bestimmen die Werte:
    std::string s = "";
    //
    for (const auto& str : vsStyleParts) {
        std::string sValue = str;
        //std::cerr << sValue << std::endl;
        if (sValue.rfind("line-style:", 0) == 0) { // pos=0 limits the search to the prefix
            sValue.erase(0, std::string("line-style:").length());
            // "line-style:normal" --> ""
            // "line-style:dotted" --> stroke-dasharray="3,3"
            // "line-style:dashed" --> stroke-dasharray="10,5"
            // "line-style:dashdotted" --> stroke-dasharray="15,3,3,3"
            if      (sValue == "dotted")
                s += "stroke-dasharray=\"3,3\" ";
            else if (sValue == "dashed")
                s += "stroke-dasharray=\"10,5\" ";
            else if (sValue == "dashdotted")
                s += "stroke-dasharray=\"15,3,3,3\" ";
        }
        if (sValue.rfind("line-weight:", 0) == 0) { // pos=0 limits the search to the prefix
            sValue.erase(0, std::string("line-weight:").length());
            // "line-weight:none" --> stroke-width:0
            // "line-weight:thin" --> stroke-width:0.25
            // "line-weight:normal" --> stroke-width:1.5
            // "line-weight:height" --> stroke-width:6
            // "line-weight:eleve" --> stroke-width:15
            if      (sValue == "none")
                s += "stroke-width=\"0\" ";
            else if (sValue == "thin")
                s += "stroke-width=\"0.25\" ";
            else if (sValue == "normal")
                s += "stroke-width=\"1.5\" ";
            else if (sValue == "height")
                s += "stroke-width=\"6\" ";
            else if (sValue == "normal")
                s += "stroke-width=\"15\" ";
            else
                s += "stroke-width=\"1.5\" ";
        }
        if (sValue.rfind("filling:", 0) == 0) { // pos=0 limits the search to the prefix
            //std::cerr << sValue << std::endl;
            sValue.erase(0, std::string("filling:").length());
            // "filling:none" --> fill:none
            s += "fill=\"" + ColorToValue(sValue) + "\" ";
        }
        if (sValue.rfind("color:", 0) == 0) { // pos=0 limits the search to the prefix
            //std::cerr << sValue << std::endl;
            sValue.erase(0, std::string("color:").length());
            // "color:black" --> stroke="black"
            s += "stroke=\"" + ColorToValue(sValue) + "\" ";
        }
    }
    // letztes Semikolon löschen:
    s.erase(s.length()-1, 1);
    s += " ";
    //
    return s;
}
//
//--- END implementation of class "BaseStyle" ------------------------------------
//



//
//--- implementation of class "ElmtDynText" ------------------------------------
//
bool ElmtDynText::ReadFromPugiNode(pugi::xml_node node)
{
    if ((std::string(node.name())) == "input") { xIsOldInput = true; }
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    z         = node.attribute("z").as_double();
    rotate    = node.attribute("rotate").as_bool(); // scheint eine Rotation um 0° zu sein!
    if (node.attribute("rotation"))
        rotation  = node.attribute("rotation").as_double();
    size      = node.attribute("size").as_double();
    text_width= node.attribute("text_width").as_double();
    if (node.attribute("text"))
        text      = node.attribute("text").as_string();
    font      = node.attribute("font").as_string();
    if (node.attribute("color"))
        color     = node.attribute("color").as_string();
    frame     = node.attribute("frame").as_bool();
    tagg      = node.attribute("tagg").as_string();
    info_name = tagg; // seems to be ...?
    uuid      = node.attribute("uuid").as_string();
    keep_visual_rotation
              = node.attribute("keep_visual_rotation").as_bool();

    // nun die Unter-Elemente:
    if (node.child("text"))
        text      = node.child("text").child_value();
    if (node.child("color"))
        color     = node.child("color").child_value();
    if (node.child("info_name"))
        info_name = node.child("info_name").child_value();

    // ToDo: Schrift und Schriftgröße bearbeiten:
    if ( font.length() > 0) {
      // Text liegt im ausführlichen Format vor
      SplitFontString();
      size = std::stod(vsFont[1]);
    } else {
      // altes Format im Element -> korrigieren!
      font = "Sans Serif,9,-1,5,50,0,0,0,0,0";
      SplitFontString();
      vsFont[1] = FormatValue(size, 0);
      CreateFontString();
    }
    return true;
}
// ---
bool ElmtDynText::WriteToPugiNode(pugi::xml_node node, size_t decimals)
{
    node.attribute("x").set_value(FormatValue(x, decimals).c_str());
    node.attribute("y").set_value(FormatValue(y, decimals).c_str());
    if (node.attribute("z"))
        node.attribute("z").set_value(FormatValue(z, 0).c_str());
    if (node.attribute("size"))
        node.attribute("size").set_value(FormatValue(size, 0).c_str());
    if (node.attribute("rotation"))
        node.attribute("rotation").set_value(FormatValue(rotation, 0).c_str());
    if (node.attribute("text"))
        node.attribute("text").set_value(text.c_str());
    if (node.attribute("font"))
        node.attribute("font").set_value(font.c_str());
    if (node.attribute("color"))
        node.attribute("color").set_value(color.c_str());
    // nun die Unter-Elemente:
    if (node.child("text"))
        node.child("text").child_value(text.c_str());
    if (node.child("color"))
        node.child("color").child_value(color.c_str());
    if (node.child("info_name"))
        node.child("info_name").child_value(info_name.c_str());
    return true;
}
// ---
std::string ElmtDynText::AsSVGstring(const uint8_t decimals)
{
    std::string s = "<text transform=\"translate(" ;
    //
    // für altes Element "input":
    if (xIsOldInput == true) {
        s += FormatValue(((x + (size/8.0)+4.05)), decimals) + ", ";
        s += FormatValue(y+0.5*size, decimals) + ")";
        if (rotation != 0.0) {
            s += " rotate(" + FormatValue(rotation, 0);
            s += " " + FormatValue(- (((size/8.0)+4.05)) , 1);
            s += " " + FormatValue(- (0.5*size), 1);
            s += ")";
        }
    }
    else {
        // für "dynamic_text":
        s += FormatValue(((x + (size/8.0)+4.05) - 0.5), 1) + ", ";
        s += FormatValue(y + (7.0/5.0*size + 26.0/5.0) - 0.5, 1) + ")";
        if (rotation != 0.0) {
            s += " rotate(" + FormatValue(rotation, 0);
            s += " " + FormatValue(- (((size/8.0)+4.05) - 0.5) , 1);
            s += " " + FormatValue(- ((7.0/5.0*size + 26.0/5.0) - 0.5), 1);
            s += ")";
        }
    }
    s += "\" ";
    // ToDo: hier könnte die Anpassung auf generische Schriftfamilien hin!!!
    s += "font-family=\"" + vsFont[0] + "\" "; // Schriftart aus Element
    s += "font-size=\"" + FormatValue(size, decimals) + "pt\" ";
    s += "fill=\"" + color + "\">";
    s += text + "</text>";
    return s;
}
// ---
void ElmtDynText::SplitFontString(void)
{
    vsFont.clear();
    // der einzelne Wert als String
    std::string value;
    // Zeile in Stringstream umkopieren:
    std::stringstream ss(font);
    // den Stringstream an Trennzeichen aufteilen ...
    while(std::getline(ss, value, ','))
    {
      // ... in den Font-Vector schieben:
      vsFont.push_back(value);
    }
}
// ---
void ElmtDynText::CreateFontString(void)
{
    font = vsFont[0] + "," + FormatValue(size, 0);
    for (size_t i=2; i<vsFont.size(); i++) {
        font += "," + vsFont[i];
    }
}
//
//--- END - implementation of class "ElmtDynText" ------------------------------
//


//
//--- implementation of class "ElmtText" --------------------------------------
//
bool ElmtText::ReadFromPugiNode(pugi::xml_node node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    size      = node.attribute("size").as_double();
    rotation  = node.attribute("rotation").as_double();
    text      = node.attribute("text").as_string();
    font      = node.attribute("font").as_string();
    if (node.attribute("color"))
        color     = node.attribute("color").as_string();

    // ToDo: Schrift und Schriftgröße bearbeiten:
    if ( font.length() > 0) {
      // Text liegt im ausführlichen Format vor
      SplitFontString();
      size = std::stod(vsFont[1]);
    } else {
      // altes Format im Element -> korrigieren!
      font = "Sans Serif,9,-1,5,50,0,0,0,0,0";
      SplitFontString();
      vsFont[1] = FormatValue(size, 0);
      CreateFontString();
    }
    return true;
}
// ---
bool ElmtText::WriteToPugiNode(pugi::xml_node node, size_t decimals)
{
    node.attribute("x").set_value(FormatValue(x, decimals).c_str());
    node.attribute("y").set_value(FormatValue(y, decimals).c_str());
    if (node.attribute("size"))
        node.attribute("size").set_value(FormatValue(size, 0).c_str());
    if (node.attribute("rotation"))
        node.attribute("rotation").set_value(FormatValue(rotation, 0).c_str());
    node.attribute("text").set_value(text.c_str());
    if (node.attribute("font"))
        node.attribute("font").set_value(font.c_str());
    if (node.attribute("color"))
        node.attribute("color").set_value(color.c_str());
    return true;
}
// ---
std::string ElmtText::AsXMLstring(const uint8_t decimals)
{
    std::string s = "<text ";
    //
    s += "x=\"" + FormatValue(x, decimals) + "\" ";
    s += "y=\"" + FormatValue(y, decimals) + "\" ";
    s += "text=\"" + text + "\" ";
    s += "font=\"" + font + "\" ";
    s += "rotation=\"" + FormatValue(rotation, 0) + "\" ";
    s += "color=\"" + color + "\" ";
    return s + "/>";
}
// ---
std::string ElmtText::AsSVGstring(const uint8_t decimals)
{
    std::string s = "<text transform=\"translate(" ;
    //
    s += FormatValue(x, decimals) + ", " + FormatValue(y, decimals) + ")";
    if (rotation != 0.0) { s += " rotate(" + FormatValue(rotation, 0) + ")"; }
    s += "\" ";
    // ToDo: hier könnte die Anpassung auf generische Schriftfamilien hin!!!
    s += "font-family=\"" + vsFont[0] + "\" ";
    s += "font-size=\"" + FormatValue(size, decimals) + "pt\" ";
    s += "fill=\"" + color + "\">";
    s += text + "</text>";
    return s;
}
// ---
void ElmtText::SplitFontString(void)
{
    vsFont.clear();
    // der einzelne Wert als String
    std::string value;
    // Zeile in Stringstream umkopieren:
    std::stringstream ss(font);
    // den Stringstream an Trennzeichen aufteilen ...
    while(std::getline(ss, value, ','))
    {
      // ... in den Font-Vector schieben:
      vsFont.push_back(value);
    }
}
// ---
void ElmtText::CreateFontString(void)
{
    font = vsFont[0] + "," + FormatValue(size, 0);
    for (size_t i=2; i<vsFont.size(); i++) {
        font += "," + vsFont[i];
    }
}
//
//--- implementation of class "ElmtText" ---------------------------------------
//




//
//--- implementation of class "ElmtArc" ----------------------------------------
//
void ElmtArc::Flip(void)
{// vertikale Spiegelung = Flip und normalisieren
    y = ((-1.0) * y) - height;
    start = (-1.0) * start;
    angle = (-1.0) * angle;
    Normalize();
}
// ---
void ElmtArc::Mirror(void)
{// horizontale Spiegelung = Mirror und normalisieren
    x = ((-1.0) * x) - width;
    start = 180 - start;
    angle = (-1) * angle;
    Normalize();
}
// ---
void ElmtArc::Normalize(void)
{// Details: siehe QET_ElementScaler
    int istart = (int)(round(start)) % 360;
    int iangle = (int)(round(angle)) % 360;
    if (istart < 0) {
        istart = (istart + 360) % 360;
    }
    if (iangle < 0) {
        istart  = (istart + iangle + 360) % 360;
        iangle *= (-1);
    }
    start = istart;
    angle = iangle;
}
// ---
void ElmtArc::SetData(double X, double Y,
                  double Width, double Height,
                  double Start, double Angle)
{// Daten übernehmen und normalisieren:
    x = X;
    y = Y;
    width = Width;
    height = Height;
    start = Start;
    angle = Angle;
    Normalize();
}
//---
bool ElmtArc::ReadFromPugiNode(pugi::xml_node node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    width     = node.attribute("width").as_double();
    height    = node.attribute("height").as_double();
    start     = node.attribute("start").as_double();
    angle     = node.attribute("angle").as_double();
    antialias = node.attribute("antialias").as_string();
    style     = node.attribute("style").as_string();

    Normalize();
    DetermineMinMax();
    return true;
}
// ---
void ElmtArc::DetermineMinMax(){
      MinX =  1e99;
      MaxX = -1e99;
      MinY =  1e99;
      MaxY = -1e99;
    // calculate points on ellipse-circumference:
    // x = a ⋅ cos(t)
    // y = b ⋅ sin(t)
    // with t = 0 .. 2*pi = ([0 .. 360]/360)*2*pi
    const double pi = 3.141592653589793116; // also in <cmath> as M_PI
    for (int i = round(start); i <= round((start + angle)); i++){
        double xx = x + (width / 2.0) * (1 + cos(i / 360.0 * 2.0 * pi));
        double yy = y + (height / 2.0) * (1 - sin(i / 360.0 * 2.0 * pi));
        MinX = std::min(MinX, xx);
        MaxX = std::max(MaxX, xx);
        MinY = std::min(MinY, yy);
        MaxY = std::max(MaxY, yy);
    }
}
// ---
bool ElmtArc::WriteToPugiNode(pugi::xml_node node, size_t decimals)
{
    node.attribute("x").set_value(FormatValue(x, decimals).c_str());
    node.attribute("y").set_value(FormatValue(y, decimals).c_str());
    node.attribute("width").set_value(FormatValue(width, decimals).c_str());
    node.attribute("height").set_value(FormatValue(height, decimals).c_str());
    node.attribute("start").set_value(FormatValue(start, 0).c_str());
    node.attribute("angle").set_value(FormatValue(angle, 0).c_str());
    node.attribute("antialias").set_value(antialias.c_str());
    node.attribute("style").set_value(style.c_str());
    return true;
}
// ---
std::string ElmtArc::AsXMLstring(const uint8_t decimals)
{
    std::string s = "<arc ";
    //
    s += "x=\"" + FormatValue(x, decimals) + "\" ";
    s += "y=\"" + FormatValue(y, decimals) + "\" ";
    s += "width=\"" + FormatValue(width, decimals) + "\" ";
    s += "height=\"" + FormatValue(height, decimals) + "\" ";
    s += "start=\"" + FormatValue(start, decimals) + "\" ";
    s += "angle=\"" + FormatValue(angle, decimals) + "\" ";
    //
    s += "style=\"" + style + "\" ";
    s += "antialias=\"" + antialias + "\" ";
    return s + "/>";
}
// ---
std::string ElmtArc::AsSVGstring(const uint8_t decimals)
{
    const double pi = 3.141592653589793116; // also in <cmath> as M_PI
    std::string s = "<path d=\"M ";
    // Punkte berechnen und in String einfügen...
    // Startpunkt:
    double sx = x + (width / 2.0) * (1 + cos(start / 360.0 * 2.0 * pi));
    double sy = y + (height / 2.0) * (1 - sin(start / 360.0 * 2.0 * pi));
    s += FormatValue(sx, decimals) + " " + FormatValue(sy, decimals);
    // Angaben für RadiusX, RadiusY, Rotation, Weg und Drehsinn:
    s += " A " + FormatValue(width/2, decimals) + " " + FormatValue(height/2, decimals) + " 0 ";
    ((angle <= 180.0) ? (s += "0") : (s += "1"));
    s += " 0 ";
    // Endpunkt:
    double ex = x + (width / 2.0) * (1 + cos((start+angle) / 360.0 * 2.0 * pi));
    double ey = y + (height / 2.0) * (1 - sin((start+angle) / 360.0 * 2.0 * pi));
    s += FormatValue(ex, decimals) + " " + FormatValue(ey, decimals) + "\" ";
    //
    s += StyleAsSVGstring(2);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtArc" ----------------------------------
//



//
//--- implementation of class "ElmtPolygon" ------------------------------------
//
bool ElmtPolygon::ReadFromPugiNode(pugi::xml_node node)
{
    if (node.attribute("closed"))
        closed    = node.attribute("closed").as_bool();
    antialias = node.attribute("antialias").as_string();
    style     = node.attribute("style").as_string();
    // walk through the attributes to read all points:
    for (pugi::xml_attribute attr: node.attributes()) {
        if (attr.name()[0]=='x') {
            std::string str = attr.name();
            str.erase(0, 1);
            InsertXat(std::stoi(str), attr.as_double());
            if (_DEBUG_) Write();
        }
        if (attr.name()[0]=='y') {
            std::string str = attr.name();
            str.erase(0, 1);
            InsertYat(std::stoi(str), attr.as_double());
            if (_DEBUG_) Write();
        }
    } // for (pugi::xml_attribute ...
    return true;
}
// ---
bool ElmtPolygon::WriteToPugiNode(pugi::xml_node node, size_t decimals)
{   // Noch nicht komplett implementiert
    for (const auto &i : polygon) {
        std::string s = "";
        s = "x" + std::to_string(std::get<0>(i));
        node.attribute(s.c_str()).set_value(FormatValue(std::get<1>(i), decimals).c_str());
        s = "y" + std::to_string(std::get<0>(i));
        node.attribute(s.c_str()).set_value(FormatValue(std::get<2>(i), decimals).c_str());
    }
    return true;
}
// ---
bool ElmtPolygon::CheckIndex(void){
   if (polygon.size() == 0 ) {
     return true;
   }
   if ( !(std::get<0>(polygon[polygon.size()-1]) == polygon.size()) ) {
     std::cerr << "max. index not equal size\n";
     return false;
   }
   for (uint64_t i=0; i<(polygon.size()-1); i++) {
     if ( (std::isnan(std::get<1>(polygon[i])))  ||
          (std::isnan(std::get<2>(polygon[i]))) ) {
       std::cerr << "Value missing at index: "<<i<<"\n";
       return false;
     }
   }
   return true;
}
// ---
void ElmtPolygon::InsertXat(uint64_t idx, double val)
{
    if (_DEBUG_) std::cerr << " insert X-val ("<<val<<") with index " << idx << "\n";
    if (polygon.size() == 0) {
        // erster Punkt --> einfach hinten dran!
        if (_DEBUG_) std::cerr << "erster Punkt vom Polygon! \n";
        polygon.push_back(std::make_tuple(idx, val, sqrt(-1)));
        return;
    } else
    if ( (idx > std::get<0>(polygon[polygon.size()-1])) ) {
        // Index ist größer als vom obersten Punkt --> hinten dran!
        if (_DEBUG_) std::cerr << "neuer hoechster Index - haenge hinten an \n";
        polygon.push_back(std::make_tuple(idx, val, sqrt(-1)));
        return;
    } else
    if (idx < std::get<0>(polygon[0])) {
        if (_DEBUG_) std::cerr << "neuer niedrigster Index kommt nach vorne! \n";
        polygon.insert(polygon.begin(), std::make_tuple(idx, val, sqrt(-1)));
        return;
    } else {
        // Punkt muss irgendwo ersetzt bzw. eingefügt werden
        for (uint64_t i=0; i<polygon.size(); i++) {
            if (_DEBUG_) std::cerr << "pruefe Index " << idx << " an Stelle " << i << " \n";
            if (idx == std::get<0>(polygon[i])) {
                if (_DEBUG_) std::cerr << "Index vorhanden - an Stelle " << i << " ersetzen \n";
                std::get<1>(polygon[i]) = val;
                return;
            } else
            if (idx < std::get<0>(polygon[i+1])) {
                if (_DEBUG_) std::cerr << "neuer Index - fuege an Stelle " << i+1 << " ein \n";
                polygon.insert(polygon.begin()+i+1, std::make_tuple(idx, val, sqrt(-1)));
                return;
            }
        } // for (uint64...
    }
}
//
void ElmtPolygon::InsertYat(uint64_t idx, double val)
{
    if (_DEBUG_) std::cerr << " insert Y-val ("<<val<<") with index " << idx << "\n";
    if (polygon.size() == 0) {
        // erster Punkt --> einfach hinten dran!
        if (_DEBUG_) std::cerr << "erster Punkt vom Polygon! \n";
        polygon.push_back(std::make_tuple(idx, sqrt(-1), val));
        return;
    } else
    if ( (idx > std::get<0>(polygon[polygon.size()-1])) ) {
        // Index ist größer als vom obersten Punkt --> hinten dran!
        if (_DEBUG_) std::cerr << "neuer hoechster Index - haenge hinten an \n";
        polygon.push_back(std::make_tuple(idx, sqrt(-1), val));
        return;
    } else
    if (idx < std::get<0>(polygon[0])) {
        if (_DEBUG_) std::cerr << "neuer niedrigster Index kommt nach vorne! \n";
        polygon.insert(polygon.begin(), std::make_tuple(idx, sqrt(-1), val));
        return;
    } else {
        // Punkt muss irgendwo ersetzt bzw. eingefügt werden
        for (uint64_t i=0; i<polygon.size(); i++) {
            if (_DEBUG_) std::cerr << "pruefe Index " << idx << " an Stelle " << i << " \n";
            if (idx == std::get<0>(polygon[i])) {
                if (_DEBUG_) std::cerr << "Index vorhanden - an Stelle " << i << " ersetzen \n";
                std::get<2>(polygon[i]) = val;
                return;
            } else
            if (idx < std::get<0>(polygon[i+1])) {
                if (_DEBUG_) std::cerr << "neuer Index - fuege an Stelle " << i+1 << " ein \n";
                polygon.insert(polygon.begin()+i+1, std::make_tuple(idx, sqrt(-1), val));
                return;
            }
        } // for (uint64...
    }
}
//
void ElmtPolygon::InsertXYat(uint64_t idx, double xval, double yval)
{
    if (_DEBUG_) std::cerr << " insert XY-val ("<<xval<<"|"<<yval<<") with index " << idx << "\n";
    if (polygon.size() == 0) {
        // erster Punkt --> einfach hinten dran!
        if (_DEBUG_) std::cerr << "erster Punkt vom Polygon! \n";
        polygon.push_back(std::make_tuple(idx, xval, yval));
        return;
    } else
    if ( (idx > std::get<0>(polygon[polygon.size()-1])) ) {
        // Index ist größer als vom obersten Punkt --> hinten dran!
        if (_DEBUG_) std::cerr << "neuer hoechster Index - haenge hinten an \n";
        polygon.push_back(std::make_tuple(idx, xval, yval));
        return;
    } else
    if (idx < std::get<0>(polygon[0])) {
        if (_DEBUG_) std::cerr << "neuer niedrigster Index kommt nach vorne! \n";
        polygon.insert(polygon.begin(), std::make_tuple(idx, xval, yval));
        return;
    } else {
        // Punkt muss irgendwo ersetzt bzw. eingefügt werden
        for (uint64_t i=0; i<polygon.size(); i++) {
            if (_DEBUG_) std::cerr << "pruefe Index " << idx << " an Stelle " << i << " \n";
            if (idx == std::get<0>(polygon[i])) {
                if (_DEBUG_) std::cerr << "Index vorhanden - an Stelle " << i << " ersetzen \n";
                std::get<1>(polygon[i]) = xval;
                std::get<2>(polygon[i]) = yval;
                return;
            } else
            if (idx < std::get<0>(polygon[i+1])) {
                if (_DEBUG_) std::cerr << "neuer Index - fuege an Stelle " << i+1 << " ein \n";
                polygon.insert(polygon.begin()+i+1, std::make_tuple(idx, xval, yval));
                return;
            }
        } // for (uint64...
    }
}
//
void ElmtPolygon::AddPoint(double x, double y)
{
    // der Index vom angefügten Punkt muß größer sein, als der vom letzten Punkt (Tuple-Position "0")
    uint64_t idx = 1;
    if (polygon.size() > 0) { idx = std::get<0>(polygon[polygon.size()-1]) + 1; }
    polygon.push_back(std::make_tuple(idx, x, y));
    if (_DEBUG_) std::cerr //<< "Anzahl Punkte: " << polygon.size() << "\n"
              << "letzte Werte: \n i: " << std::get<0>(polygon[polygon.size()-1]) << "\n"
              << " X = " << std::get<1>(polygon[polygon.size()-1]) << "\n"
              << " Y = " << std::get<2>(polygon[polygon.size()-1]) << "\n";
}
//
void ElmtPolygon::AddPoint(uint64_t idx, double x, double y)
{
    InsertXYat(idx, x, y);
}
// ---
void ElmtPolygon::Flip(void)
{// vertikale Spiegelung = Flip
    for (uint64_t i=0; i<polygon.size(); i++) {
        std::get<2>(polygon[i]) = std::get<2>(polygon[i]) * (-1);
    }
}
// ---
void ElmtPolygon::Mirror(void)
{// horizontale Spiegelung = Mirror
    for (uint64_t i=0; i<polygon.size(); i++) {
        std::get<1>(polygon[i]) = std::get<1>(polygon[i]) * (-1);
    }
}
// ---
void ElmtPolygon::Scale(const double factX, const double factY)
{
    for (uint64_t i=0; i<polygon.size(); i++) {
        std::get<1>(polygon[i]) = std::get<1>(polygon[i]) * factX;
        std::get<2>(polygon[i]) = std::get<2>(polygon[i]) * factY;
    }
}
// ---
std::string ElmtPolygon::AsXMLstring(const uint8_t decimals)
{
    if (polygon.size() == 0) return "<polygon />";
    std::string s = "<polygon ";
    //
    for (uint64_t i=0; i<polygon.size(); i++) {
        s += "x" + std::to_string(std::get<0>(polygon[i])) + "=\"" + FormatValue(std::get<1>(polygon[i]), decimals) + "\" ";
        s += "y" + std::to_string(std::get<0>(polygon[i])) + "=\"" + FormatValue(std::get<2>(polygon[i]), decimals) + "\" ";
    }
    //
    s += "style=\"" + style + "\" ";
    s += "antialias=\"" + antialias + "\" ";
    return s + "/>";
}
// ---
std::string ElmtPolygon::AsSVGstring(const uint8_t decimals)
{
    if (polygon.size() == 0) return "<polygon />";
    std::string s = "<polygon points=\"";
    if (closed == false) s = "<polyline points=\"";
    //
    for (uint64_t i=0; i<polygon.size(); i++) {
        s += FormatValue(std::get<1>(polygon[i]), decimals) + ",";
        s += FormatValue(std::get<2>(polygon[i]), decimals) + " ";
    }
    s[s.length()-1] = '\"';
    s += " ";
    //
    s += StyleAsSVGstring(2);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtPolygon" ------------------------------
//





//
//--- implementation of class "ElmtEllipse" ------------------------------------
//
bool ElmtEllipse::ReadFromPugiNode(pugi::xml_node node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    if (std::string(node.name()) == "ellipse") {
        width     = node.attribute("width").as_double();
        height    = node.attribute("height").as_double();
    }
    if (std::string(node.name()) == "circle") {
        width     = node.attribute("diameter").as_double();
        height    = width;
    }
    antialias = node.attribute("antialias").as_string();
    style     = node.attribute("style").as_string();
    return true;
}
// ---
bool ElmtEllipse::WriteToPugiNode(pugi::xml_node node, size_t decimals)
{
    node.attribute("x").set_value(FormatValue(x, decimals).c_str());
    node.attribute("y").set_value(FormatValue(y, decimals).c_str());
    if (std::string(node.name()) == "ellipse") {
        node.attribute("width").set_value(FormatValue(width, decimals).c_str());
        node.attribute("height").set_value(FormatValue(height, decimals).c_str());
    }
    if (std::string(node.name()) == "circle") {
        node.attribute("diameter").set_value(FormatValue(width, decimals).c_str());
    }
    node.attribute("antialias").set_value(antialias.c_str());
    node.attribute("style").set_value(style.c_str());
    return true;
}
// ---
std::string ElmtEllipse::AsXMLstring(const uint8_t decimals)
{
    std::string s = "<ellipse ";
    //
    s += "x=\"" + FormatValue(x, decimals) + "\" ";
    s += "y=\"" + FormatValue(y, decimals) + "\" ";
    s += "width=\"" + FormatValue(width, decimals) + "\" ";
    s += "height=\"" + FormatValue(height, decimals) + "\" ";
    //
    s += "style=\"" + style + "\" ";
    s += "antialias=\"" + antialias + "\" ";
    return s + "/>";
}
// ---
std::string ElmtEllipse::AsSVGstring(const uint8_t decimals)
{
    std::string s = "<ellipse ";
    //
    s += "cx=\"" + FormatValue((x + width/2), decimals) + "\" ";
    s += "cy=\"" + FormatValue((y + height/2), decimals) + "\" ";
    s += "rx=\"" + FormatValue(width/2, decimals) + "\" ";
    s += "ry=\"" + FormatValue(height/2, decimals) + "\" ";
    //
    s += StyleAsSVGstring(2);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtEllipse" ------------------------------
//





//
//--- implementation of class "ElmtRect" ---------------------------------------
//
bool ElmtRect::ReadFromPugiNode(const pugi::xml_node node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    rx        = node.attribute("rx").as_double();
    ry        = node.attribute("ry").as_double();
    width     = node.attribute("width").as_double();
    height    = node.attribute("height").as_double();
    antialias = node.attribute("antialias").as_string();
    style     = node.attribute("style").as_string();
    return true;
}
// ---
bool ElmtRect::WriteToPugiNode(const pugi::xml_node node, const size_t decimals)
{
    node.attribute("x").set_value(FormatValue(x, decimals).c_str());
    node.attribute("y").set_value(FormatValue(y, decimals).c_str());
    node.attribute("rx").set_value(FormatValue(rx, decimals).c_str());
    node.attribute("ry").set_value(FormatValue(ry, decimals).c_str());
    node.attribute("width").set_value(FormatValue(width, decimals).c_str());
    node.attribute("height").set_value(FormatValue(height, decimals).c_str());
    node.attribute("antialias").set_value(antialias.c_str());
    node.attribute("style").set_value(style.c_str());
    return true;
}
// ---
std::string ElmtRect::AsXMLstring(const uint8_t decimals)
{
    std::string s = "<rect ";
    //
    s += "x=\"" + FormatValue(x, decimals) + "\" ";
    s += "y=\"" + FormatValue(y, decimals) + "\" ";
    s += "width=\"" + FormatValue(width, decimals) + "\" ";
    s += "height=\"" + FormatValue(height, decimals) + "\" ";
    s += "rx=\"" + FormatValue(rx, decimals) + "\" ";
    s += "ry=\"" + FormatValue(ry, decimals) + "\" ";
    //
    s += "style=\"" + style + "\" ";
    s += "antialias=\"" + antialias + "\" ";
    return s + "/>";
}
// ---
std::string ElmtRect::AsSVGstring(const uint8_t decimals)
{
    std::string s = "<rect ";
    //
    s += "x=\"" + FormatValue(x, decimals) + "\" ";
    s += "y=\"" + FormatValue(y, decimals) + "\" ";
    s += "width=\"" + FormatValue(width, decimals) + "\" ";
    s += "height=\"" + FormatValue(height, decimals) + "\" ";
    if (rx > 0.0) s += "rx=\"" + FormatValue(rx, decimals) + "\" ";
    if (ry > 0.0) s += "ry=\"" + FormatValue(ry, decimals) + "\" ";
    //
    s += StyleAsSVGstring(2);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtRect" ---------------------------------
//




//
//--- implementation of class "ElmtLine" ---------------------------------------
//
bool ElmtLine::ReadFromPugiNode(pugi::xml_node node)
{
    length1   = node.attribute("length1").as_double();
    length2   = node.attribute("length2").as_double();
    end1      = node.attribute("end1").as_string();
    end2      = node.attribute("end2").as_string();
    antialias = node.attribute("antialias").as_string();
    style     = node.attribute("style").as_string();

    InsertXYat(1, node.attribute("x1").as_double(), node.attribute("y1").as_double());
    InsertXYat(2, node.attribute("x2").as_double(), node.attribute("y2").as_double());

    return true;
}
// ---
bool ElmtLine::WriteToPugiNode(pugi::xml_node node, size_t decimals)
{   // Noch nicht komplett implementiert
    node.attribute("x1").set_value(FormatValue(std::get<1>(polygon[0]), decimals).c_str());
    node.attribute("x2").set_value(FormatValue(std::get<1>(polygon[1]), decimals).c_str());
    node.attribute("y1").set_value(FormatValue(std::get<2>(polygon[0]), decimals).c_str());
    node.attribute("y2").set_value(FormatValue(std::get<2>(polygon[1]), decimals).c_str());
    return true;
}
// ---
std::string ElmtLine::AsXMLstring(const uint8_t decimals)
{
    if (polygon.size() == 0) return "<line />";
    std::string s = "<line ";
    //
    for (uint64_t i=0; i<polygon.size(); i++) {
        s += "x" + std::to_string(std::get<0>(polygon[i])) + "=\"" + FormatValue(std::get<1>(polygon[i]), decimals) + "\" ";
        s += "y" + std::to_string(std::get<0>(polygon[i])) + "=\"" + FormatValue(std::get<2>(polygon[i]), decimals) + "\" ";
    }
    //
    s += "end1=\"" + end1 + "\" ";
    s += "length1=\"" + FormatValue(length1, decimals) + "\" ";
    s += "end2=\"" + end2 + "\" ";
    s += "length2=\"" + FormatValue(length2, decimals) + "\" ";
    s += "style=\"" + style + "\" ";
    s += "antialias=\"" + antialias + "\" ";
    return s + "/>";
}
// ---
std::string ElmtLine::AsSVGstring(const uint8_t decimals)
{
/* Linie mit Pfeil-Enden:
   Enden müssen in "defs"-Sektion definiert werden und können dann
   z.B. über eine "use"-Anweisung verwendet werden. (vgl "terminal")
   Problem: in QET können die Enden parametriert werden und müssen
   sowieso passend rotiert werden ... im Moment: zu aufwändig!
  <defs>
    <marker id="startarrow" markerWidth="10" markerHeight="7"
    refX="10" refY="3.5" orient="auto">
      <polygon points="10 0, 10 7, 0 3.5" fill="red" />
    </marker>
    <marker id="endarrow" markerWidth="10" markerHeight="7"
    refX="0" refY="3.5" orient="auto" markerUnits="strokeWidth">
        <polygon points="0 0, 10 3.5, 0 7" fill="red" />
    </marker>
  </defs>
  <line x1="100" y1="50" x2="250" y2="50" stroke="#000" stroke-width="8"
  marker-end="url(#endarrow)" marker-start="url(#startarrow)" />

 */
    if (polygon.size() == 0) return "<line />";
    std::string s = "<line ";
    //
    for (uint64_t i=0; i<polygon.size(); i++) {
        s += "x" + std::to_string(std::get<0>(polygon[i])) + "=\"" + FormatValue(std::get<1>(polygon[i]), decimals) + "\" ";
        s += "y" + std::to_string(std::get<0>(polygon[i])) + "=\"" + FormatValue(std::get<2>(polygon[i]), decimals) + "\" ";
    }
    //
    s += StyleAsSVGstring(2);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtLine" -------------------------------------
//



//
//--- implementation of class "ElmtTerminal" -----------------------------------
//
bool ElmtTerminal::ReadFromPugiNode(pugi::xml_node node)
{
    x           = node.attribute("x").as_double();
    y           = node.attribute("y").as_double();
    orientation = node.attribute("orientation").as_string();
    type        = node.attribute("type").as_string();
    name        = node.attribute("name").as_string();
    uuid        = node.attribute("uuid").as_string();

    // wir prüfen auf aktuelle Vorgabewerte:
    if (uuid.length() == 0) { CreateNewUUID(); }
    if (type.length() == 0) { type = "Generic"; }
    return true;
}
// ---
bool ElmtTerminal::WriteToPugiNode(pugi::xml_node node)
{
    node.attribute("x").set_value(FormatValue(x, 0).c_str());
    node.attribute("y").set_value(FormatValue(y, 0).c_str());
    node.attribute("orientation").set_value(orientation.c_str());
    if (node.attribute("type"))
        node.attribute("type").set_value(type.c_str());
    if (node.attribute("name"))
        node.attribute("name").set_value(name.c_str());
    if (node.attribute("uuid"))
        node.attribute("uuid").set_value(uuid.c_str());

    x           = node.attribute("x").as_double();
    y           = node.attribute("y").as_double();
    orientation = node.attribute("orientation").as_string();
    type        = node.attribute("type").as_string();
    name        = node.attribute("name").as_string();
    uuid        = node.attribute("uuid").as_string();
    // wir prüfen auf aktuelle Vorgabewerte:
    if (uuid.length() == 0) { CreateNewUUID(); }
    if (type.length() == 0) { type = "Generic"; }
    return true;
}
// ---
std::string ElmtTerminal::AsXMLstring(const uint8_t decimals)
{
    std::string s = "<terminal ";
    //
    s += "x=\"" + FormatValue(x, 0) + "\" ";
    s += "y=\"" + FormatValue(y, 0) + "\" ";
    s += "orientation=\"" + orientation + "\" ";
    s += "type=\"" + type + "\" ";
    s += "name=\"" + name + "\" ";
    s += "uuid=\"" + uuid + "\" ";
    //
    return s + "/>";
}
// ---
std::string ElmtTerminal::AsSVGstring(const uint8_t decimals)
{
    std::string s = "<use xlink:href=\"#terminal\" ";
    //
    s += "x=\"" + FormatValue(x, 0) + "\" ";
    s += "y=\"" + FormatValue(y, 0) + "\" ";
    // Fallunterscheidung "orientation":
    if ( (orientation == "e") || (orientation == "E") ) {
        s += "transform=\"rotate(90 ";
        s += FormatValue(x, 0) + " ";
        s += FormatValue(y, 0) + ")\" ";
    }
    else if ( (orientation == "s") || (orientation == "S") ) {
        s += "transform=\"rotate(180 ";
        s += FormatValue(x, 0) + " ";
        s += FormatValue(y, 0) + ")\" ";
    }
    else if ( (orientation == "w") || (orientation == "W") ) {
        s += "transform=\"rotate(270 ";
        s += FormatValue(x, 0) + " ";
        s += FormatValue(y, 0) + ")\" ";
    }
    else /* muss "n" oder "N" sein! */ {
        s += ""; // keine Rotation!
    }
    //s += "type=\"" + type + "\" ";
    //s += "name=\"" + name + "\" ";
    //s += "uuid=\"" + uuid + "\" ";
    //
    return s + "/>";
}
//
//--- END - implementation of class "ElmtTerminal" -----------------------------
//






/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
double getFontSize(const std::string str)
{
  std::string s = str;
  std::string v = "";  // Teilstring vor der Größenangabe
  std::string n = "";  // Teilstring nach der Größenangabe

//  std::cerr << "Font-String: -->" << s << "<-- \n";
  std::size_t found = s.find(",");
  // delete up to first ","
  if (found != std::string::npos) {
    v = s.substr(0, found+1);
//    std::cerr << "Teilstring v: -->" << v << "<-- \n";
    s.erase(s.begin(), s.begin()+found+1);
//    std::cerr << "Font-String: -->" << s << "<-- \n";
    found = s.find(",");
    // delete everything behind ","
    if (found != std::string::npos) {
      n = s.substr(found, s.length()-found);
//      std::cerr << "Teilstring n: -->" << n << "<-- \n";
      s.erase(s.begin()+found, s.end()-found+1);
    }
  } else { return 8.999;}
//  std::cerr << "Font-Size: -->" << s << "<-- \n";
  return std::stod(s);
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/


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
//
//--- END - implementation of class "RectMinMax" -------------------------------
//



/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
// Farbnamen von QET in RGB-Werte umwandeln
std::string ColorToValue (const std::string Color){
  // hier die Liste der bekannten Farben:
  std::map<std::string, std::string> ColorMap = {
    {"white", "#FFFFFF"},
    {"black", "#000000"},
    {"green", "#00FF00"},
    {"red", "#FF0000"},
    {"blue", "#0000FF"},
    {"gray", "#A0A0A4"},
    {"grey", "#A0A0A4"},
    {"brun", "#A52A2A"},
    {"brown", "#A52A2A"},
    {"yellow", "#FFFF00"},
    {"cyan", "#00FFFF"},
    {"magenta", "#FF00FF"},
    {"lightgray", "#D3D3D3"},
    {"orange", "#FFA500"},
    {"purple", "#A020F0"},
    {"HTMLPinkPink", "#FFC0CB"},
    {"HTMLPinkLightPink", "#FFB6C1"},
    {"HTMLPinkHotPink", "#FF69B4"},
    {"HTMLPinkDeepPink", "#FF1493"},
    {"HTMLPinkPaleVioletRed", "#DB7093"},
    {"HTMLPinkMediumVioletRed", "#C71585"},
    {"HTMLRedLightSalmon", "#FFA07A"},
    {"HTMLRedSalmon", "#FA8072"},
    {"HTMLRedDarkSalmon", "#E9967A"},
    {"HTMLRedLightCoral", "#F08080"},
    {"HTMLRedIndianRed", "#CD5C5C"},
    {"HTMLRedCrimson", "#DC143C"},
    {"HTMLRedFirebrick", "#B22222"},
    {"HTMLRedDarkRed", "#8B0000"},
    {"HTMLRedRed", "#FF0000"},
    {"HTMLOrangeOrangeRed", "#FF4500"},
    {"HTMLOrangeTomato", "#FF6347"},
    {"HTMLOrangeCoral", "#FF7F50"},
    {"HTMLOrangeDarkOrange", "#FF8C00"},
    {"HTMLOrangeOrange", "#FFA500"},
    {"HTMLYellowYellow", "#FFFF00"},
    {"HTMLYellowLightYellow", "#FFFFE0"},
    {"HTMLYellowLemonChiffon", "#FFFACD"},
    {"HTMLYellowLightGoldenrodYellow", "#FAFAD2"},
    {"HTMLYellowPapayaWhip", "#FFEFD5"},
    {"HTMLYellowMoccasin", "#FFE4B5"},
    {"HTMLYellowPeachPuff", "#FFDAB9"},
    {"HTMLYellowPaleGoldenrod", "#EEE8AA"},
    {"HTMLYellowKhaki", "#F0E68C"},
    {"HTMLYellowDarkKhaki", "#BDB76B"},
    {"HTMLYellowGold", "#FFD700"},
    {"HTMLBrownCornsilk", "#FFF8DC"},
    {"HTMLBrownBlanchedAlmond", "#FFEBCD"},
    {"HTMLBrownBisque", "#FFE4C4"},
    {"HTMLBrownNavajoWhite", "#FFDEAD"},
    {"HTMLBrownWheat", "#F5DEB3"},
    {"HTMLBrownBurlywood", "#DEB887"},
    {"HTMLBrownTan", "#D2B48C"},
    {"HTMLBrownRosyBrown", "#BC8F8F"},
    {"HTMLBrownSandyBrown", "#F4A460"},
    {"HTMLBrownGoldenrod", "#DAA520"},
    {"HTMLBrownDarkGoldenrod", "#B8860B"},
    {"HTMLBrownPeru", "#CD853F"},
    {"HTMLBrownChocolate", "#D2691E"},
    {"HTMLBrownSaddleBrown", "#8B4513"},
    {"HTMLBrownSienna", "#A0522D"},
    {"HTMLBrownBrown", "#A52A2A"},
    {"HTMLBrownMaroon", "#B03060"},
    {"HTMLGreenDarkOliveGreen", "#556B2F"},
    {"HTMLGreenOlive", "#808000"},
    {"HTMLGreenOliveDrab", "#6B8E23"},
    {"HTMLGreenYellowGreen", "#9ACD32"},
    {"HTMLGreenLimeGreen", "#32CD32"},
    {"HTMLGreenLime", "#C0FF00"},
    {"HTMLGreenLawnGreen", "#7CFC00"},
    {"HTMLGreenChartreuse", "#7FFF00"},
    {"HTMLGreenGreenYellow", "#ADFF2F"},
    {"HTMLGreenSpringGreen", "#00FF7F"},
    {"HTMLGreenMediumSpringGreen", "#00FA9A"},
    {"HTMLGreenLightGreen", "#90EE90"},
    {"HTMLGreenPaleGreen", "#98FB98"},
    {"HTMLGreenDarkSeaGreen", "#8FBC8F"},
    {"HTMLGreenMediumAquamarine", "#66CDAA"},
    {"HTMLGreenMediumSeaGreen", "#3CB371"},
    {"HTMLGreenSeaGreen", "#2E8B57"},
    {"HTMLGreenForestGreen", "#228B22"},
    {"HTMLGreenGreen", "#00FF00"},
    {"HTMLGreenDarkGreen", "#006400"},
    {"HTMLCyanAqua", "#00FFFF"},
    {"HTMLCyanCyan", "#00FFFF"},
    {"HTMLCyanLightCyan", "#E0FFFF"},
    {"HTMLCyanPaleTurquoise", "#AFEEEE"},
    {"HTMLCyanAquamarine", "#7FFFD4"},
    {"HTMLCyanTurquoise", "#40E0D0"},
    {"HTMLCyanMediumTurquoise", "#48D1CC"},
    {"HTMLCyanDarkTurquoise", "#00CED1"},
    {"HTMLCyanLightSeaGreen", "#20B2AA"},
    {"HTMLCyanCadetBlue", "#5F9EA0"},
    {"HTMLCyanDarkCyan", "#008B8B"},
    {"HTMLCyanTeal", "#008080"},
    {"HTMLBlueLightSteelBlue", "#B0C4DE"},
    {"HTMLBluePowderBlue", "#B0E0E6"},
    {"HTMLBlueLightBlue", "#ADD8E6"},
    {"HTMLBlueSkyBlue", "#87CEEB"},
    {"HTMLBlueLightSkyBlue", "#87CEFA"},
    {"HTMLBlueDeepSkyBlue", "#00BFFF"},
    {"HTMLBlueDodgerBlue", "#1E90FF"},
    {"HTMLBlueCornflowerBlue", "#6495ED"},
    {"HTMLBlueSteelBlue", "#4682B4"},
    {"HTMLBlueRoyalBlue", "#4169E1"},
    {"HTMLBlueBlue", "#0000FF"},
    {"HTMLBlueMediumBlue", "#0000CD"},
    {"HTMLBlueDarkBlue", "#00008B"},
    {"HTMLBlueNavy", "#000080"},
    {"HTMLBlueMidnightBlue", "#191970"},
    {"HTMLPurpleLavender", "#E6E6FA"},
    {"HTMLPurpleThistle", "#D8BFD8"},
    {"HTMLPurplePlum", "#DDA0DD"},
    {"HTMLPurpleViolet", "#EE82EE"},
    {"HTMLPurpleOrchid", "#DA70D6"},
    {"HTMLPurpleFuchsia", "#FF00FF"},
    {"HTMLPurpleMagenta", "#FF00FF"},
    {"HTMLPurpleMediumOrchid", "#BA55D3"},
    {"HTMLPurpleMediumPurple", "#9370DB"},
    {"HTMLPurpleBlueViolet", "#8A2BE2"},
    {"HTMLPurpleDarkViolet", "#9400D3"},
    {"HTMLPurpleDarkOrchid", "#9932CC"},
    {"HTMLPurpleDarkMagenta", "#8B008B"},
    {"HTMLPurplePurple", "#800080"},
    {"HTMLPurpleIndigo", "#4B0082"},
    {"HTMLPurpleDarkSlateBlue", "#483D8B"},
    {"HTMLPurpleSlateBlue", "#6A5ACD"},
    {"HTMLPurpleMediumSlateBlue", "#7B68EE"},
    {"HTMLWhiteWhite", "#FFFFFF"},
    {"HTMLWhiteSnow", "#FFFAFA"},
    {"HTMLWhiteHoneydew", "#F0FFF0"},
    {"HTMLWhiteMintCream", "#F5FFFA"},
    {"HTMLWhiteAzure", "#F0FFFF"},
    {"HTMLWhiteAliceBlue", "#F0F8FF"},
    {"HTMLWhiteGhostWhite", "#F8F8FF"},
    {"HTMLWhiteWhiteSmoke", "#F5F5F5"},
    {"HTMLWhiteSeashell", "#FFF5EE"},
    {"HTMLWhiteBeige", "#F5F5DC"},
    {"HTMLWhiteOldLace", "#FDF5E6"},
    {"HTMLWhiteFloralWhite", "#FFFAF0"},
    {"HTMLWhiteIvory", "#FFFFF0"},
    {"HTMLWhiteAntiqueWhite", "#FAEBD7"},
    {"HTMLWhiteLinen", "#FAF0E6"},
    {"HTMLWhiteLavenderBlush", "#FFF0F5"},
    {"HTMLWhiteMistyRose", "#FFE4E1"},
    {"HTMLGrayGainsboro", "#DCDCDC"},
    {"HTMLGrayLightGray", "#D3D3D3"},
    {"HTMLGraySilver", "#C0C0C0"},
    {"HTMLGrayDarkGray", "#A9A9A9"},
    {"HTMLGrayGray", "#808080"},
    {"HTMLGrayDimGray", "#696969"},
    {"HTMLGrayLightSlateGray", "#778899"},
    {"HTMLGraySlateGray", "#708090"},
    {"HTMLGrayDarkSlateGray", "#2F4F4F"},
    {"HTMLGrayBlack", "#000000"},
    {"none", "none"}
  };
  // Suche und Ausgabe der Farbe:
  std::string Value = ColorMap[Color];
  if ((Value == "none") || (Value.length() == 7))
    return Value;
  else
    return "#696969";
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/