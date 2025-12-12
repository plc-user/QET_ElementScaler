/*
 * Copyright (c) 2022-2025 plc-user
 *
 * It is NOT allowed to use this software or any of the
 * associated files as sample data for the purposes of
 * building machine learning models.
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

#include <iomanip>      // for IO-Operations
#include <string>       // for string-handling
#include <sstream>      // for String-Streams
#include <algorithm>    // for std::sort

//
//--- implementation of class "DefinitionLine" ---------------------------------
//
void DefinitionLine::ReadFromPugiNode(pugi::xml_node node)
{
    version   = node.attribute("version").as_string();
    link_type = node.attribute("link_type").as_string();
    type      = node.attribute("type").as_string();
    width     = node.attribute("width").as_int();
    height    = node.attribute("height").as_int();
    hotspot_x = node.attribute("hotspot_x").as_int();
    hotspot_y = node.attribute("hotspot_y").as_int();
}
// ---
void DefinitionLine::ReCalc(RectMinMax XYMinMax) {
// size and hotspots have to be re-calculated after scaling!
    int w = (int)round(XYMinMax.width());
    int h = (int)round(XYMinMax.height());

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
    hotspot_x = -((int)round(XYMinMax.xmin() - (xmargin/2)));
    hotspot_y = -((int)round(XYMinMax.ymin() - (ymargin/2)));
}
// ---
void DefinitionLine::WriteToPugiNode(pugi::xml_node node)
{   // sort attributes:
    node.remove_attribute("version");
    node.append_attribute("version").set_value(version);
    node.remove_attribute("type");
    node.append_attribute("type").set_value(type);
    node.remove_attribute("link_type");
    node.append_attribute("link_type").set_value(link_type);
    node.remove_attribute("width");
    node.append_attribute("width").set_value(std::to_string(width));
    node.remove_attribute("height");
    node.append_attribute("height").set_value(std::to_string(height));
    node.remove_attribute("hotspot_x");
    node.append_attribute("hotspot_x").set_value(std::to_string(hotspot_x));
    node.remove_attribute("hotspot_y");
    node.append_attribute("hotspot_y").set_value(std::to_string(hotspot_y));
}
//
//--- END - implementation of class "DefinitionLine" ---------------------------
//



//
//--- implementation of class "NamesList" --------------------------------------
//
void NamesList::ReadFromPugiNode(pugi::xml_node node)
{
    node = node.first_child();
    for (; node; node = node.next_sibling()){
        std::string lang = trim(node.attribute("lang").as_string());
        std::string name = trim(node.child_value());
        AddName(lang, name);
        // keine sort-Funktion: Das macht die std::map automatisch!
    }
}
void NamesList::WriteToPugiNode(pugi::xml_node node)
{
    // erstmal alle Namen löschen:
    while(node.remove_child("name"));
    // und jetzt kommen die Namen sortiert wieder rein:
    for (const auto& [key, value] : names) {
        node.append_child("name").append_attribute("lang").set_value(key);
        node.last_child().text().set(value);
    }
}
//
//--- END - implementation of class "NamesList" --------------------------------
//



//
//--- implementation of class "ElementInfo" --------------------------------------
//
void ElementInfo::ReadFromPugiNode(pugi::xml_node node)
{
    node = node.first_child();
    for (; node; node = node.next_sibling()){
        std::string name = node.attribute("name").as_string();
        std::string text = trim(node.child_value());
        bool show        = node.attribute("show").as_bool();
        AddInfo(EInfo{name, text, show});
    }
    // abschließend die Liste sortieren nach "name":
    std::sort(info.begin(), info.end(),
        [](const EInfo& v1, const EInfo& v2) {
            return v1.name < v2.name;
            }
    );
}
void ElementInfo::WriteToPugiNode(pugi::xml_node node)
{
    const std::string nodeChild = "elementInformation";
    // erstmal alle informationen löschen, wenn's sie gibt:
    if (node.child(nodeChild))
        while(node.remove_child(nodeChild));
    // und jetzt kommen die Namen sortiert wieder rein:
    for (const auto& [name, text, show] : info) {
        if (text.empty()) continue; // do not save empty lines!
        node.append_child(nodeChild).append_attribute("show").set_value((int)show);
        node.last_child().append_attribute("name").set_value(name);
        node.last_child().text().set(trim(text));
    }
    if (!node.child(nodeChild)) {
        std::cerr << "no \"elementInformations\" --> remove node\n";
        node.parent().remove_child("elementInformations");
    }
}
//
//--- END - implementation of class "ElementInfo" --------------------------------
//



//
//--- implementation of class Author-Information -------------------------------
//
void AuthorInfo::ReadFromPugiNode(pugi::xml_node node)
{
    author = trim(node.child_value());
}
void AuthorInfo::UpdatePugiNode(pugi::xml_node node)
{
    ReadFromPugiNode(node);
    if (author.find("QET_ElementScaler") == std::string::npos)
        author += "\nedited with \"QET_ElementScaler\"";
    WriteToPugiNode(node);
}
void AuthorInfo::WriteToPugiNode(pugi::xml_node node)
{
    node.text().set(trim(author));
}
//
//--- END - implementation of Author-Information -------------------------------
//



//
//--- implementation of class "BaseStyle" --------------------------------------
//
std::string BaseStyle::StyleAsSVGstring(const size_t& decimals)
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
        std::string val = str;
        // wir bestimmen die einzelnen Linien-Parameter:
        if (val.rfind("line-style:", 0) == 0) { // pos=0 limits the search to the prefix
            val.erase(0, std::string("line-style:").length());
            lineStyle = val;
        }
        if (val.rfind("line-weight:", 0) == 0) { // pos=0 limits the search to the prefix
            val.erase(0, std::string("line-weight:").length());
            lineWeight = val;
        }
        if (val.rfind("color:", 0) == 0) { // pos=0 limits the search to the prefix
            val.erase(0, std::string("color:").length());
            lineColor = val;
        }
        if (val.rfind("filling:", 0) == 0) { // pos=0 limits the search to the prefix
            val.erase(0, std::string("filling:").length());
            lineFilling = val;
        }
    }

    // und nun den SVG-String bauen:
    double dStrokeWidth = GetLineWidth();

    // "line-style:normal" --> ""
    // "line-style:dotted" --> stroke-dasharray="1,3" oder stroke-dasharray="1% 1%"
    // "line-style:dashed" --> stroke-dasharray="4,3"
    // "line-style:dashdotted" --> stroke-dasharray="8,2,1,2"
    // aber abhängig von der Strichbreite!
    if      (lineStyle == "dotted")
        s += "stroke-dasharray=\"" + FormatValue((1.0 * dStrokeWidth), decimals) + "," +
                                     FormatValue((2.0 * dStrokeWidth), decimals) + "\" ";
    else if (lineStyle == "dashed")
        s += "stroke-dasharray=\"" + FormatValue((4.0 * dStrokeWidth), decimals) + "," +
                                     FormatValue((2.0 * dStrokeWidth), decimals) + "\" ";
    else if (lineStyle == "dashdotted")
        s += "stroke-dasharray=\"" + FormatValue((4.0 * dStrokeWidth), decimals) + "," +
                                     FormatValue((2.0 * dStrokeWidth), decimals) + "," +
                                     FormatValue((1.0 * dStrokeWidth), decimals) + "," +
                                     FormatValue((2.0 * dStrokeWidth), decimals) + "\" ";

    // Linienbreite liegt bereits als Zahlwert vor (s.o.):
    s += "stroke-width=\"" + FormatValue((dStrokeWidth), decimals) + "\" ";

    // "filling:none" --> fill:none
    s += "fill=\"" + ColorToValue(lineFilling) + "\" ";

    // "color:black" --> stroke="black"
    s += "stroke=\"" + ColorToValue(lineColor) + "\" ";

    // letztes Semikolon löschen:
    s.erase(s.length()-1, 1);
    s += " ";
    //
    return s;
}
//
//--- END implementation of class "BaseStyle" ----------------------------------
//



//
//--- implementation of class "ElmtInput" --------------------------------------
//
void ElmtInput::ConvertToDynText(pugi::xml_node& node)
/*
in QElectroTech wird diese input-Zeile:
<input x="0" y="0" rotation="0" rotate="true" size="3" text="Size  3" tagg="none"/>
in diesen dynamic_text umgewandelt:
<dynamic_text rotation="0" uuid="{4b25c058-4dd4-4e7c-a05b-2ebed344197a}" Halignment="AlignLeft" y="-7.5" text_from="UserText" Valignment="AlignTop" x="0" z="1" font="Liberation Sans,3,-1,5,25,0,0,0,0,0,Regular" text_width="-1" keep_visual_rotation="false" frame="false">
    <text>Size  3</text>
DAS wollen wir hier auch machen!
*/
{
    for (pugi::xml_attribute_iterator ait = node.attributes_begin(); ait != node.attributes_end(); ++ait)
    {
        if (_DEBUG_)
            std::cerr << "  " << ait->name() << "=" << ait->value();
        if ((std::string)ait->name() == "x") {
            x = node.attribute("x").as_double();
        }
        if ((std::string)ait->name() == "y") {
            y = node.attribute(ait->name()).as_double();
        }
        if ((std::string)ait->name() == "rotate") {
            rotate = node.attribute(ait->name()).as_bool();
        }
        if ((std::string)ait->name() == "rotation") {
            rotation = node.attribute(ait->name()).as_double();
        }
        if ((std::string)ait->name() == "size") {
            size = node.attribute(ait->name()).as_double();
        }
        if ((std::string)ait->name() == "tagg") {
            // wird zu: "text_from" und "info_name"
            tagg = node.attribute(ait->name()).as_string();
        }
        if ((std::string)ait->name() == "text") {
            text = node.attribute(ait->name()).as_string();
        }
    }
    if (_DEBUG_)
        std::cerr << "\n";
    // die bekannten und eingelesenen Attribute löschen:
    node.remove_attribute("x");
    node.remove_attribute("y");
    node.remove_attribute("rotate");
    node.remove_attribute("rotation");
    node.remove_attribute("size");
    node.remove_attribute("tagg");
    node.remove_attribute("text");
    // die neue Position in Abhängigkeit des Rotationswinkels:
    x = x + (7.0/8.0 * size + 39.0/8.0) * sin(toRad<double>(rotation));
    y = y - (7.0/8.0 * size + 39.0/8.0) * cos(toRad<double>(rotation));
    // nun setzen wir die neuen Attribute:
    node.append_attribute("x").set_value(FormatValue(x, 2));
    node.append_attribute("y").set_value(FormatValue(y, 2));
    node.append_attribute("z").set_value("1");
    node.append_attribute("rotation").set_value(FormatValue(rotation, 0));
    node.append_attribute("font").set_value("Liberation Sans," + FormatValue(size, 0) + ",-1,5,25,0,0,0,0,0,Regular");
    node.append_attribute("uuid").set_value("{" + CreateUUID(false) + "}");
    if (tagg == "label" ) {
        node.append_attribute("text_from").set_value("ElementInfo");
    } else {
        node.append_attribute("text_from").set_value("UserText");
    }
    node.append_attribute("frame").set_value("false");
    node.append_attribute("Valignment").set_value("AlignTop");
    node.append_attribute("Halignment").set_value("AlignLeft");
    node.append_attribute("text_width").set_value(-1);
    node.append_attribute("keep_visual_rotation").set_value("false");
    // nun die Unter-Elemente:
    node.append_child("text").text().set(text);
    if (tagg == "label" ) {
        node.append_child("info_name").text().set("label");
    } else {
        // no child here
    }
    // zum Schluß den Node umbenennen:
    node.set_name("dynamic_text");
}
//
//--- END - implementation of class "ElmtDynText" ------------------------------
//



//
//--- implementation of class "ElmtDynText" ------------------------------------
//
void ElmtDynText::ReadFromPugiNode(pugi::xml_node& node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    z         = node.attribute("z").as_double();
    if (node.attribute("rotate"))
        rotate    = node.attribute("rotate").as_bool(); // scheint eine Rotation um 0° zu sein!
    if (node.attribute("rotation"))
        rotation  = node.attribute("rotation").as_double();
    if (node.attribute("Halignment"))
        Halignment= node.attribute("Halignment").as_string();
    if (node.attribute("Valignment"))
        Valignment= node.attribute("Valignment").as_string();
    if (node.attribute("font_size"))
        size      = node.attribute("font_size").as_double();
    if (node.attribute("size"))
        size      = node.attribute("size").as_double();
    if (node.attribute("text_width"))
        text_width= node.attribute("text_width").as_double();
    if (node.attribute("text"))
        text      = node.attribute("text").as_string();
    if (node.attribute("text_from"))
        text_from = node.attribute("text_from").as_string();
    if (node.attribute("font"))
        font      = node.attribute("font").as_string();
    if (node.attribute("color"))
        color     = node.attribute("color").as_string();
    if (node.attribute("frame"))
        frame     = node.attribute("frame").as_bool();
    if (node.attribute("uuid")) {
        uuid      = node.attribute("uuid").as_string();
        if (uuid.length() != 38)
            uuid  = "{" + CreateUUID(false) + "}";
    } else {
        uuid      = "{" + CreateUUID(false) + "}";
        node.append_attribute("uuid").set_value(uuid);
    }
    if (node.attribute("keep_visual_rotation"))
        keep_visual_rotation
                  = node.attribute("keep_visual_rotation").as_bool();

    // nun die Unter-Elemente:
    if (node.child("text"))
        text           = node.child("text").child_value();
    if (node.child("color"))
        color          = node.child("color").child_value();
    if (node.child("info_name"))
        info_name      = node.child("info_name").child_value();
    if (node.child("composite_text"))
        composite_text = node.child("composite_text").child_value();

    // ToDo: Schrift und Schriftgröße bearbeiten:
    if ( font.length() > 0) {
      // Text liegt im ausführlichen Format vor
      SplitFontString();
      size = std::stod(vsFont[1]);
    } else {
      // altes Format im Element -> korrigieren!
      font = "Liberation Sans,9,-1,5,50,0,0,0,0,0,Regular";
      SplitFontString();
      vsFont[1] = FormatValue(size, 0);
      CreateFontString();
    }
}
// ---
void ElmtDynText::WriteToPugiNode(pugi::xml_node& node, const size_t& decimals)
{
    // Attribute löschen und wieder anfügen zum sortieren
    node.remove_attribute("x");
    node.append_attribute("x").set_value(FormatValue(x, decimals));
    node.remove_attribute("y");
    node.append_attribute("y").set_value(FormatValue(y, decimals));
    if (node.attribute("z")) {
        node.remove_attribute("z");
        node.append_attribute("z").set_value(FormatValue(z, 0));
    }
    if (node.attribute("text_width")) {
        node.remove_attribute("text_width");
        if ( text_width < 0.0 ) { text_width = -1.0; }
        node.append_attribute("text_width").set_value(FormatValue(text_width, 0));
    }
    if (node.attribute("Halignment")) {
        node.remove_attribute("Halignment");
        node.append_attribute("Halignment").set_value(Halignment);
    }
    if (node.attribute("Valignment")) {
        node.remove_attribute("Valignment");
        node.append_attribute("Valignment").set_value(Valignment);
    }
    if (node.attribute("frame")) {
        node.remove_attribute("frame");
        node.append_attribute("frame").set_value(frame);
    }
    if (node.attribute("rotate")) {
        node.remove_attribute("rotate");
        node.append_attribute("rotate").set_value(rotate);
    }
    if (node.attribute("rotation")) {
        node.remove_attribute("rotation");
        node.append_attribute("rotation").set_value(FormatValue(rotation, 0));
    }
    if (node.attribute("keep_visual_rotation")) {
        node.remove_attribute("keep_visual_rotation");
        node.append_attribute("keep_visual_rotation").set_value((keep_visual_rotation ? "true" : "false"));
    }
    if (node.attribute("text_from")) {
        node.remove_attribute("text_from");
        node.append_attribute("text_from").set_value(text_from);
    }
    if (node.attribute("size")) {
        node.remove_attribute("size");
        node.append_attribute("size").set_value(FormatValue(size, 0));
    }
    if (node.attribute("font_size")) {
        node.remove_attribute("font_size");
        node.append_attribute("font_size").set_value(FormatValue(size, 0));
    }
    if (node.attribute("text")) { // noch nötig??
        node.remove_attribute("text");
        node.append_attribute("text").set_value(text);
    }
    if (node.attribute("uuid")) {
        node.remove_attribute("uuid");
        node.append_attribute("uuid").set_value(uuid);
    }
    if (node.attribute("font")) {
        node.remove_attribute("font");
        node.append_attribute("font").set_value(font);
    }
    if (node.attribute("color")) {
        node.remove_attribute("color");
        node.append_attribute("color").set_value(color);
    }
    // nun die Unter-Elemente:
    if (node.child("text"))
        node.child("text").text().set(text);
    if (node.child("color"))
        node.child("color").text().set(color);
    if (node.child("info_name"))
        node.child("info_name").text().set(info_name);
    if (node.child("composite_text"))
        node.child("composite_text").text().set(composite_text);
}
// ---
void ElmtDynText::Rot90(void){
    RotPoint90(x, y);
    rotation += 90;
    if (rotation >= 360.0){
        rotation -= 360;
    }
}
// ---
std::string ElmtDynText::AsSVGstring(const size_t& decimals)
{
    // Positionen x, y, Rotationspunkt, für die beiden Varianten berechnen
    double posx;
    double posy;
    double rotx;
    double roty;
    std::string s = "";
    // Position und Rotationspunkt berechnen:
    posx = ((x + (size/8.0)+4.05) - 0.5);
    posy = y + (7.0/5.0*size + 26.0/5.0) - 0.5;
    rotx = (-1) * (((size/8.0)+4.05) - 0.5);
    roty = (-1) * ((7.0/5.0*size + 26.0/5.0) - 0.5);
    // zuerst wird auf mehrzeiligen Text geprüft und behandelt:
    if ( (!(text.find("\n") == std::string::npos)) ||
         (!(text.find("\r") == std::string::npos)) )  {
        //std::cerr << "\"dynamic_text\" mit Umbruch erkannt!\n";
        std::vector<std::string> vsText;
        MultiLineText(text, vsText);
        s += "<text y=\"" + FormatValue(posy, decimals) + "\" transform=\"rotate(";
        s += FormatValue(rotation, decimals) + " " + FormatValue(x, decimals) + " " + FormatValue(y, decimals) + ")\" ";
        s += "font-family=\"" + FontToFontFamily(vsFont[0]) + "\" ";
        s += "font-size=\"" + FormatValue(size, 0) + "pt\" ";
        s += "fill=\"" + color + "\">\n";
        for (size_t i=0; i<vsText.size(); i++) {
            s += "      <tspan x=\"" + FormatValue(posx, decimals) + "\" dy=\"" + FormatValue(((i>0)*1.4), decimals) + "em\">";
            s += TextToEntity(vsText[i]) + "</tspan>\n";
        }
        s += "      </text>";
        // für Multiline-Text in "dynamic_text" war's das
        return s;
    }

    // bei einzeiligem Text bleibt alles wie's war:
    s += "<text transform=\"translate(" ;
    s += FormatValue(posx, 1) + ", ";
    s += FormatValue(posy, 1) + ")";
    if (std::abs(rotation) > 0.1) {
        s += " rotate(" + FormatValue(rotation, 1);
        s += " " + FormatValue(rotx, 1);
        s += " " + FormatValue(roty, 1);
        s += ")";
    }
    s += "\" ";
    // wir nutzen hier generische Schriftfamilien!!!
    s += "font-family=\"" + FontToFontFamily(vsFont[0]) + "\" ";
    s += "font-size=\"" + FormatValue(size, decimals) + "pt\" ";
    s += "fill=\"" + color + "\">";
    s += TextToEntity(text) + "</text>";
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
//--- implementation of class "ElmtText" ---------------------------------------
//
void ElmtText::ReadFromPugiNode(pugi::xml_node& node)
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
      font = "Liberation Sans,9,-1,5,50,0,0,0,0,0,Regular";
      SplitFontString();
      vsFont[1] = FormatValue(size, 0);
      CreateFontString();
    }
}
// ---
void ElmtText::WriteToPugiNode(pugi::xml_node& node, const size_t& decimals)
{   // sort attributes by removing and re-adding
    node.remove_attribute("text");
    node.prepend_attribute("text").set_value(text);
    node.remove_attribute("x");
    node.append_attribute("x").set_value(FormatValue(x, decimals));
    node.remove_attribute("y");
    node.append_attribute("y").set_value(FormatValue(y, decimals));
    if (node.attribute("size")) {
        node.remove_attribute("size");
        node.append_attribute("size").set_value(FormatValue(size, 0));
    }
    if (node.attribute("rotation")) {
        node.remove_attribute("rotation");
        node.append_attribute("rotation").set_value(FormatValue(rotation, 0));
    }
    if (node.attribute("font")) {
        node.remove_attribute("font");
        node.append_attribute("font").set_value(font);
    }
    if (node.attribute("color")) {
        node.remove_attribute("color");
        node.append_attribute("color").set_value(color);
    }
}
// ---
void ElmtText::Rot90(void){
    RotPoint90(x, y);
    rotation += 90;
    if (rotation >= 360.0){
        rotation -= 360;
    }
}
// ---
std::string ElmtText::AsSVGstring(const size_t& decimals)
{
    // mehrzeiliger Text wird in einem "Vector of String" abgelegt
    // als Trennzeichen für die Teil-Strings: "\n" und "\r"
    if ( (!(text.find("\n") == std::string::npos)) ||
         (!(text.find("\r") == std::string::npos)) )  {
        std::vector<std::string> vsText;
        MultiLineText(text, vsText);
        // wir brauchen einen String für den Rückgabewert:
        std::string s = "";
        s += "<text y=\"" + FormatValue(y, decimals) + "\" transform=\"rotate(";
        s += FormatValue(rotation, decimals) + " " + FormatValue(x, decimals) + " " + FormatValue(y, decimals) + ")\" ";
        s += "font-family=\"" + FontToFontFamily(vsFont[0]) + "\" ";
        s += "font-size=\"" + FormatValue(size, 0) + "pt\" ";
        s += "fill=\"" + color + "\">\n";
        for (size_t i=0; i<vsText.size(); i++) {
            s += "      <tspan x=\"" + FormatValue(x, decimals) + "\" dy=\"" + FormatValue(((i>0)*1.4), decimals) + "em\">";
            s += TextToEntity(vsText[i]) + "</tspan>\n";
        }
        s += "      </text>";
        return s;
    }
    else {
        // einzeiligen Text behandeln wir separat:
        std::string s = "<text transform=\"translate(" ;
        //
        s += FormatValue(x, decimals) + ", " + FormatValue(y, decimals) + ")";
        if (rotation != 0.0) { s += " rotate(" + FormatValue(rotation, decimals) + ")"; }
        s += "\" ";
        // wir nutzen hier generische Schriftfamilien!!!
        s += "font-family=\"" + FontToFontFamily(vsFont[0]) + "\" ";
        s += "font-size=\"" + FormatValue(size, decimals) + "pt\" ";
        s += "fill=\"" + color + "\">";
        s += TextToEntity(text) + "</text>";
        return s;
    }
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
void ElmtArc::ReadFromPugiNode(pugi::xml_node& node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    width     = node.attribute("width").as_double();
    height    = node.attribute("height").as_double();
    start     = node.attribute("start").as_double();
    angle     = node.attribute("angle").as_double();
    antialias = node.attribute("antialias").as_bool();
    style     = node.attribute("style").as_string();

    Normalize();
    DetermineMinMax();
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
    for (int i = (int)round(start); i <= (int)round((start + angle)); i++){
        double xx = x + (width / 2.0) * (1 + cos(toRad<double>(i)));
        double yy = y + (height / 2.0) * (1 - sin(toRad<double>(i)));
        MinX = std::min(MinX, xx);
        MaxX = std::max(MaxX, xx);
        MinY = std::min(MinY, yy);
        MaxY = std::max(MaxY, yy);
    }
}
// ---
void ElmtArc::WriteToPugiNode(pugi::xml_node& node, const size_t& decimals)
{   // sort attributes:
    node.remove_attribute("x");
    node.append_attribute("x").set_value(FormatValue(x, decimals));
    node.remove_attribute("y");
    node.append_attribute("y").set_value(FormatValue(y, decimals));
    node.remove_attribute("width");
    node.append_attribute("width").set_value(FormatValue(width, decimals));
    node.remove_attribute("height");
    node.append_attribute("height").set_value(FormatValue(height, decimals));
    node.remove_attribute("start");
    node.append_attribute("start").set_value(FormatValue(start, 0));
    node.remove_attribute("angle");
    node.append_attribute("angle").set_value(FormatValue(angle, 0));
    node.remove_attribute("style");
    node.append_attribute("style").set_value(style);
    node.remove_attribute("antialias");
    node.append_attribute("antialias").set_value(antialias);
}
// ---
void ElmtArc::Rot90(void)
{
    start += 270.0;
    Normalize();
    RotPos90(x, y, width, height);
    SwapWidhHeight();
}
// ---
std::string ElmtArc::AsSVGstring(const size_t& decimals)
{
    std::string s = "<path d=\"M ";
    // Punkte berechnen und in String einfügen...
    // Startpunkt:
    double sx = x + (width / 2.0) * (1 + cos(toRad<double>(start)));
    double sy = y + (height / 2.0) * (1 - sin(toRad<double>(start)));
    s += FormatValue(sx, decimals) + " " + FormatValue(sy, decimals);
    // Angaben für RadiusX, RadiusY, Rotation, Weg und Drehsinn:
    s += " A " + FormatValue(width/2, decimals) + " " + FormatValue(height/2, decimals) + " 0 ";
    ((angle <= 180.0) ? (s += "0") : (s += "1"));
    s += " 0 ";
    // Endpunkt:
    double ex = x + (width / 2.0) * (1 + cos(toRad<double>(start+angle)));
    double ey = y + (height / 2.0) * (1 - sin(toRad<double>(start+angle)));
    s += FormatValue(ex, decimals) + " " + FormatValue(ey, decimals) + "\" ";
    //
    s += StyleAsSVGstring(decimals);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtArc" ----------------------------------
//



//
//--- implementation of class "ElmtPolygon" ------------------------------------
//
bool ElmtPolygon::ReadFromPugiNode(pugi::xml_node& node)
{
    if (node.attribute("closed"))
        closed    = node.attribute("closed").as_bool();
    antialias = node.attribute("antialias").as_bool();
    style     = node.attribute("style").as_string();
    // insert values in map to automatically sort:
    std::map<uint64_t, double> valX;
    std::map<uint64_t, double> valY;
    uint64_t maxIX = 0; // max. Index for X
    uint64_t maxIY = 0; // max. Index for Y
    // walk through the attributes to read all points:
    for (pugi::xml_attribute attr: node.attributes()) {
        if (attr.name()[0]=='x') {
            std::string str = attr.name();
            str.erase(0, 1);
            if (((std::string)attr.as_string() == "nan") || (!(std::isfinite(attr.as_double())))) {
                std::cerr << " * * * Polygon-Point with invalid value!\n";
            } else {
                if ((uint64_t)std::stoi(str) > maxIX) { maxIX = std::stoi(str); }
                valX[std::stoi(str)] = attr.as_double();
            }
            if (_DEBUG_) Write();
        }
        if (attr.name()[0]=='y') {
            std::string str = attr.name();
            str.erase(0, 1);
            if (((std::string)attr.as_string() == "nan") || (!(std::isfinite(attr.as_double())))) {
                std::cerr << " * * * Polygon-Point with invalid value!\n";
            } else {
                if ((uint64_t)std::stoi(str) > maxIY) { maxIY = std::stoi(str); }
                valY[std::stoi(str)] = attr.as_double();
            }
            if (_DEBUG_) Write();
        }
    } // for (pugi::xml_attribute ...
    if (    (valX.size() == valY.size())     // beide maps müssen gleich groß sein
         && (valX.size() >= 2)               // Polygon muss mehr als einen Punkt haben
         && (valX.find(valX.size())->first == maxIX)  // höchster Index und Größe stimmen überein
         && (valY.find(valY.size())->first == maxIY)  // höchster Index und Größe stimmen überein
        )
    {
        // umkopieren in vector of PolyPoint:
        for (uint64_t i=0; i<valX.size(); i++) {
            polygon.push_back(PolyPoint(i+1, valX.at(i+1), valY.at(i+1)));
        }
        return true;
    } else {
        std::cerr << "Polygon not konsistent! \n";
        return false;
    }
    // Check the Polygon:
    return CheckIndex(node.name());
}
// ---
void ElmtPolygon::WriteToPugiNode(pugi::xml_node& node, const size_t& decimals)
{   // wir sortieren die Attribute
    // zuerst sind die Polygon-Punkte dran:
    for (const auto &pt : polygon) {
        std::string s = "";
        s = "x" + std::to_string(pt.i);
        node.remove_attribute(s);   // to sort the polygon-points...
        node.append_attribute(s).set_value(FormatValue(pt.x, decimals));
        s = "y" + std::to_string(pt.i);
        node.remove_attribute(s);   // to sort the polygon-points...
        node.append_attribute(s).set_value(FormatValue(pt.y, decimals));
    }
    // dann die sonstigen Eigenschaften des Polygons:
    if (node.attribute("closed"))
        node.remove_attribute("closed");
    // "closed" ist nur drin, wenn "false":
    if (closed == false)
        node.append_attribute("closed").set_value(closed);
    // antialias kommt dazu?
    if (node.attribute("antialias")) {
        node.remove_attribute("antialias");
        node.append_attribute("antialias").set_value(antialias);
    }
    // einen Style hat das Polygon auch:
    if (node.attribute("style")) {
        node.remove_attribute("style");
        node.append_attribute("style").set_value(style);
    }
}
// ---
bool ElmtPolygon::CheckIndex(const std::string sType){
   // Polygon soll mindestens zwei Punkte haben!
   if (polygon.size() < 2) {
     std::cerr << "Remove " << sType << " with one or less points!\n";
     return false;
   }
   // Höchster Index ist gleich Anzahl Punkte?
   if ( !((polygon[polygon.size()-1].i) == polygon.size())) {
     std::cerr << "Remove " << sType << ": indexing not correct (point missing)\n";
     return false;
   }
   for (uint64_t i=0; i<(polygon.size()-1); i++) {
     if ( (std::isnan(polygon[i].x))  ||
          (std::isnan(polygon[i].y)) ) {
       std::cerr << "Remove " << sType << ": Value missing at index: "<< i+1 <<"\n";
       return false;
     }
   }
   return true;
}
// ---
void ElmtPolygon::Flip(void)
{// vertikale Spiegelung = Flip
    for (auto &pt : polygon) {
        pt.y *= (-1);
    }
}
// ---
void ElmtPolygon::Mirror(void)
{// horizontale Spiegelung = Mirror
    for (auto &pt : polygon) {
        pt.x *= (-1);
    }
}
// ---
void ElmtPolygon::Rot90(void)
{// drehen um 90° im Uhrzeigersinn
    for (auto &pt : polygon) {
        RotPoint90(pt.x, pt.y);
    }
}
// ---
void ElmtPolygon::Scale(const double factX, const double factY)
{
    for (auto &pt: polygon) {
        pt.x *= factX;
        pt.y *= factY;
    }
}
// ---
void ElmtPolygon::Move(const double dx, const double dy)
{   // add the delta-values to all X- and Y-values
    for (auto &pt: polygon) {
        pt.x += dx;
        pt.y += dy;
    }
}
// ---
std::string ElmtPolygon::AsSVGstring(const size_t& decimals)
{
    if (polygon.size() == 0) return "<polygon />";
    std::string s = "<polygon points=\"";
    if (closed == false) s = "<polyline points=\"";
    //
    for (auto &pt: polygon) {
        s += FormatValue(pt.x, decimals) + ",";
        s += FormatValue(pt.y, decimals) + " ";
    }
    s[s.length()-1] = '\"';
    s += " ";
    //
    s += StyleAsSVGstring(decimals);
    return s + "/>";
}
// ---
void ElmtPolygon::CleanUp(pugi::xml_node node, const double epsilon)
// prüfen, ob direkt aufeinander folgende Polygon-Punkte gleich sind -> den zweiten löschen!
// und aus dem pugi-node den letzten Punkt löschen. Zum Schluß die Punkte neu durchnummerieren
{
    bool xDeletedPoint = false;
    // das gesamte Polygon durchgehen:
    for (uint64_t i=(polygon.size()-2); i>0; i--) {
        double diffx = std::abs((polygon[i].x) - (polygon[i+1].x));
        double diffy = std::abs((polygon[i].y) - (polygon[i+1].y));
        if ((diffx <= epsilon) && (diffy <= epsilon)) {
            // letzten Punkt aus dem Pugi-Node löschen
            std::string s = "x" + std::to_string(polygon.size());
            node.remove_attribute(s);   // to get rid of double points
            s = "y" + std::to_string(polygon.size());
            node.remove_attribute(s);   // to get rid of double points
            // den Punkt i+1 aus vector entfernen:
            polygon.erase(polygon.begin() + i+1);
            xDeletedPoint = true;
        }
    }
    // und nun mal schauen, ob das Polygon durch Punkte geschlossen wurde:
    double diffx = std::abs(polygon[0].x - polygon[polygon.size()-1].x);
    double diffy = std::abs(polygon[0].y - polygon[polygon.size()-1].y);
    if ((diffx <= epsilon) && (diffy <= epsilon)) {
        // letzten Punkt aus dem Pugi-Node löschen
        std::string s = "x" + std::to_string(polygon.size());
        node.remove_attribute(s);   // to get rid of double points
        s = "y" + std::to_string(polygon.size());
        node.remove_attribute(s);   // to get rid of double points
        // den letzten Punkt aus vector entfernen:
        polygon.erase(--polygon.end());
        // nun ist aber das Polygon geschlossen:
        closed = true;
        xDeletedPoint = true;
    }
    // Polygon muss neu durchnummeriert werden?
    if (xDeletedPoint == true) {
        for (uint64_t i=0; i<polygon.size(); i++) {
            polygon[i].i = i + 1;
        }
    }
}
//
//--- END - implementation of class "ElmtPolygon" ------------------------------
//





//
//--- implementation of class "ElmtEllipse" ------------------------------------
//
void ElmtEllipse::ReadFromPugiNode(pugi::xml_node& node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    if (node.attribute("width"))
        width     = node.attribute("width").as_double();
    else
        node.append_attribute("width");
    if (node.attribute("height"))
        height    = node.attribute("height").as_double();
    else
        node.append_attribute("height");
    if (node.attribute("diameter")) {
        width     = node.attribute("diameter").as_double();
        height    = width;
        node.remove_attribute("diameter");
    }
    antialias = node.attribute("antialias").as_bool();
    style     = node.attribute("style").as_string();
}
// ---
void ElmtEllipse::WriteToPugiNode(pugi::xml_node& node, const size_t& decimals)
{   // sort attributes:
    if (node.attribute("x")){
        node.remove_attribute("x");
        node.append_attribute("x").set_value(FormatValue(x, decimals));
    }
    if (node.attribute("y")){
        node.remove_attribute("y");
        node.append_attribute("y").set_value(FormatValue(y, decimals));
    }
    if (node.attribute("width")){
        node.remove_attribute("width");
        node.append_attribute("width").set_value(FormatValue(width, decimals));
    }
    if (node.attribute("height")){
        node.remove_attribute("height");
        node.append_attribute("height").set_value(FormatValue(height, decimals));
    }
    if (node.attribute("style")){
        node.remove_attribute("style");
        node.append_attribute("style").set_value(style);
    }
    if (node.attribute("antialias")){
        node.remove_attribute("antialias");
        node.append_attribute("antialias").set_value(antialias);
    }
}
// ---
void ElmtEllipse::Rot90(void)
{
    RotPos90(x, y, width, height);
    SwapWidhHeight();
}
// ---
std::string ElmtEllipse::AsSVGstring(const size_t& decimals)
{
    std::string s = "<ellipse ";
    //
    s += "cx=\"" + FormatValue((x + width/2), decimals) + "\" ";
    s += "cy=\"" + FormatValue((y + height/2), decimals) + "\" ";
    s += "rx=\"" + FormatValue(width/2, decimals) + "\" ";
    s += "ry=\"" + FormatValue(height/2, decimals) + "\" ";
    //
    s += StyleAsSVGstring(decimals);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtEllipse" ------------------------------
//





//
//--- implementation of class "ElmtRect" ---------------------------------------
//
void ElmtRect::ReadFromPugiNode(pugi::xml_node& node)
{
    x         = node.attribute("x").as_double();
    y         = node.attribute("y").as_double();
    rx        = node.attribute("rx").as_double();
    ry        = node.attribute("ry").as_double();
    width     = node.attribute("width").as_double();
    height    = node.attribute("height").as_double();
    antialias = node.attribute("antialias").as_bool();
    style     = node.attribute("style").as_string();
}
// ---
void ElmtRect::WriteToPugiNode(pugi::xml_node& node, const size_t& decimals)
{
    node.remove_attribute("x");
    node.append_attribute("x").set_value(FormatValue(x, decimals));
    node.remove_attribute("y");
    node.append_attribute("y").set_value(FormatValue(y, decimals));
    node.remove_attribute("width");
    node.append_attribute("width").set_value(FormatValue(width, decimals));
    node.remove_attribute("height");
    node.append_attribute("height").set_value(FormatValue(height, decimals));
    node.remove_attribute("rx");
    node.append_attribute("rx").set_value(FormatValue(rx, decimals));
    node.remove_attribute("ry");
    node.append_attribute("ry").set_value(FormatValue(ry, decimals));
    node.remove_attribute("style");
    node.append_attribute("style").set_value(style);
    node.remove_attribute("antialias");
    node.append_attribute("antialias").set_value(antialias);
}
// ---
void ElmtRect::Rot90(void)
{
    RotPos90(x, y, width, height);
    std::swap(width, height);
    std::swap(rx, ry);
}
// ---
std::string ElmtRect::AsSVGstring(const size_t& decimals)
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
    s += StyleAsSVGstring(decimals);
    return s + "/>";
}
//
//--- END - implementation of class "ElmtRect" ---------------------------------
//




//
//--- implementation of class "ElmtLine" ---------------------------------------
//
bool ElmtLine::ReadFromPugiNode(pugi::xml_node& node)
{
    length1   = node.attribute("length1").as_double();
    length2   = node.attribute("length2").as_double();
    end1      = node.attribute("end1").as_string();
    end2      = node.attribute("end2").as_string();
    antialias = node.attribute("antialias").as_bool();
    style     = node.attribute("style").as_string();
    // Gibt es die Attribute überhaupt???
    if ((node.attribute("x1") && node.attribute("x2") &&
         node.attribute("y1") && node.attribute("y2"))   == false) {
        std::cerr << "Remove incomplete " << node.name() << "\n";
        return false;
        }
    // die Daten in den Vector:
    polygon.push_back(PolyPoint(1, node.attribute("x1").as_double(), node.attribute("y1").as_double()));
    polygon.push_back(PolyPoint(2, node.attribute("x2").as_double(), node.attribute("y2").as_double()));
    return CheckIndex(node.name());
}
// ---
void ElmtLine::WriteToPugiNode(pugi::xml_node& node, const size_t& decimals)
{   // sort attributes
    node.remove_attribute("x1");
    node.append_attribute("x1").set_value(FormatValue(polygon[0].x, decimals));
    node.remove_attribute("y1");
    node.append_attribute("y1").set_value(FormatValue(polygon[0].y, decimals));
    node.remove_attribute("x2");
    node.append_attribute("x2").set_value(FormatValue(polygon[1].x, decimals));
    node.remove_attribute("y2");
    node.append_attribute("y2").set_value(FormatValue(polygon[1].y, decimals));
    node.remove_attribute("end1");
    node.append_attribute("end1").set_value(end1);
    node.remove_attribute("end2");
    node.append_attribute("end2").set_value(end2);
    node.remove_attribute("length1");
    node.append_attribute("length1").set_value(FormatValue(length1, decimals));
    node.remove_attribute("length2");
    node.append_attribute("length2").set_value(FormatValue(length2, decimals));
    node.remove_attribute("style");
    node.append_attribute("style").set_value(style);
    node.remove_attribute("antialias");
    node.append_attribute("antialias").set_value(antialias);
}
// ---
std::string ElmtLine::AsSVGstring(const size_t& decimals)
{
    if (polygon.size() == 0) return "<line />";

    // Hier müssen die Enden mit verwurstet werden: Alles in eine Gruppierung
    std::string s = "<g ";
    // hier kommen die Linienattribute mit rein
    s += StyleAsSVGstring(decimals);
    // Anfang auf x1|y1 verschieben:
    s += "transform=\"translate(" + FormatValue(polygon[0].x, decimals) + ","
                                  + FormatValue(polygon[0].y, decimals) + ")";
    // und falls die Linie eine Schräge ist, auch rotieren:
    if ( (GetAngle() > 0.05) || (GetAngle() < -0.05))
        s += " rotate(" + FormatValue(GetAngle(), decimals) + ")";
    // Gruppierungs-Anfang beenden:
    s += "\" >\n";
    // wir bestimmen Anfang und Ende der realen Linie in Abhängigkeit der Enden:
    double llength = GetLength();
    double lwidth  = GetLineWidth();
    double x1      = 0.0;
    double x2      = llength;
    if      (end1 == "none") {
            x1 +=  0.0;
    }
    else if (end1 == "simple") {
            x1 += (lwidth / 2.0);
            // einfachen Pfeil (polyline) zeichnen
            s += "      <polyline points=\"" + FormatValue(length1, decimals) + ","
                                             + FormatValue(length1, decimals)
                                             + " 0,0 "
                                             + FormatValue(length1, decimals) + ","
                                             + "-" + FormatValue(length1, decimals)
                                             + "\" />\n";
    }
    else if (end1 == "triangle") {
            x1 += length1 + (lwidth / 2.0);
            // dreieckigen Pfeil (polygon) zeichnen
            s += "      <polygon points=\"" + FormatValue(length1, decimals) + ","
                                            + FormatValue(length1, decimals)
                                            + " 0,0 "
                                            + FormatValue(length1, decimals) + ","
                                            + "-" + FormatValue(length1, decimals)
                                            + "\" />\n";
    }
    else if (end1 == "circle") {
            x1 = (length1 * 2) + (lwidth / 2.0);
            // Kringel zeichnen
            s += "      <ellipse cx=\"" + FormatValue(length1, decimals) + "\" cy=\"0\""
                                      + " rx=\"" + FormatValue(length1, decimals) + "\""
                                      + " ry=\"" + FormatValue(length1, decimals) + "\""
                                      + " />\n";
    }
    else if (end1 == "diamond") {
            x1 = (length1 * 2) + (lwidth / 2.0);
            // Karo zeichnen
            s += "      <polygon points=\"0,0 "
                                      + FormatValue( length1, decimals) + ","
                                      + FormatValue(length1, decimals) + " "
                                      + FormatValue((2 * length1), decimals) + ",0 "
                                      + FormatValue( length1, decimals) + ","
                                      + "-" + FormatValue(length1, decimals)
                                      + "\" />\n";
    }

    // ... und nun das Ende der realen Linie bestimmen und die Enden zeichnen:
    if      (end2 == "none") {
            x2 -=  0.0;
    }
    else if (end2 == "simple") {
            x2 -= (lwidth / 2.0);
            // einfachen Pfeil (polyline) zeichnen
            s += "      <polyline points=\"" + FormatValue((llength-length2), decimals) + ","
                                             + FormatValue(length2, decimals) + " "
                                             + FormatValue(llength, decimals) + ",0 "
                                             + FormatValue((llength-length2), decimals) + ","
                                             + "-" + FormatValue(length2, decimals)
                                             + "\" />\n";
    }
    else if (end2 == "triangle") {
            x2 -= (length2 + (lwidth / 2.0));
            // dreieckigen Pfeil (polygon) zeichnen
            s += "      <polygon points=\"" + FormatValue(llength, decimals) + ",0 "
                                            + FormatValue((llength-length2), decimals) + ","
                                            + FormatValue(length2, decimals) + " "
                                            + FormatValue((llength-length2), decimals) + ","
                                            + "-" + FormatValue(length2, decimals)
                                            + "\" />\n";
    }
    else if (end2 == "circle") {
            x2 -= ((length2 * 2) + (lwidth / 2.0));
            // Kringel zeichnen
            s += "      <ellipse cx=\"" + FormatValue((llength - length2), decimals) + "\" cy=\"0\""
                                        + " rx=\"" + FormatValue(length2, decimals) + "\""
                                        + " ry=\"" + FormatValue(length2, decimals) + "\""
                                        + " />\n";
    }
    else if (end2 == "diamond") {
            x2 -= ((length2 * 2) + (lwidth / 2.0));
            // Karo zeichnen
            s += "      <polygon points=\""
                                      + FormatValue( llength, decimals) + ",0 "
                                      + FormatValue((llength - length2), decimals) + ","
                                      + FormatValue(length2, decimals) + " "
                                      + FormatValue((llength - 2 * length2), decimals) + ",0 "
                                      + FormatValue((llength - length2), decimals) + ","
                                      + "-" + FormatValue(length2, decimals)
                                      + "\" />\n";
    }

    // nun noch die eigentliche Linie dazu:
    s += "      <line x1=\"" + FormatValue(x1, decimals) + "\" y1=\"0\" x2=\""
                             + FormatValue(x2, decimals) + "\" y2=\"0\" />\n";
    // Gruppierung ist hier beendet:
    s += "      </g>";
    // und die Funktion auch!
    return s;
}
//
//--- END - implementation of class "ElmtLine" ---------------------------------
//



//
//--- implementation of class "ElmtTerminal" -----------------------------------
//
void ElmtTerminal::ReadFromPugiNode(pugi::xml_node& node)
{
    x           = node.attribute("x").as_double();
    y           = node.attribute("y").as_double();
    orientation = node.attribute("orientation").as_string();
    type        = node.attribute("type").as_string();
    if (type.length() == 0) { type = "Generic"; }
    name        = node.attribute("name").as_string();
    uuid        = node.attribute("uuid").as_string();
    if (uuid.length() != 38) {
        uuid  = "{" + CreateUUID(false) + "}";
        node.remove_attribute("uuid");
        node.prepend_attribute("uuid").set_value(uuid);
    }
}
// ---
void ElmtTerminal::WriteToPugiNode(pugi::xml_node& node)
{
    // to achieve a constant order we need to remove and append attributes
    if (node.attribute("uuid")) {
        node.remove_attribute("uuid");
        node.prepend_attribute("uuid").set_value(uuid);
        } else {
        node.prepend_attribute("uuid").set_value(("{" + CreateUUID(false) + "}"));
        }
    if (node.attribute("name")) {
        node.remove_attribute("name");
        node.append_attribute("name").set_value(name);
        } else {
        node.append_attribute("name").set_value("");
        }
    node.remove_attribute("x");
    node.append_attribute("x").set_value(FormatValue(x, 0));
    node.remove_attribute("y");
    node.append_attribute("y").set_value(FormatValue(y, 0));
    node.remove_attribute("orientation");
    node.append_attribute("orientation").set_value(orientation);
    if (node.attribute("type")) {
        node.remove_attribute("type");
        node.append_attribute("type").set_value(type);
        } else {
        node.append_attribute("type").set_value("Generic");
        }
}
// ---
void ElmtTerminal::Rot90(void){
    RotPoint90(x, y);
    switch (orientation[0]){
    case 'n': orientation = "e"; break;
    case 'e': orientation = "s"; break;
    case 's': orientation = "w"; break;
    case 'w': orientation = "n"; break;
    }
}
// ---
std::string ElmtTerminal::AsSVGstring(const size_t& decimals)
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



//
//--- implementation of class "RectMinMax" -------------------------------------
//
RectMinMax::RectMinMax(){
    xMin = 0.0;
    xMax = 0.0;
    yMin = 0.0;
    yMax = 0.0;
}
RectMinMax::RectMinMax(const double x, const double y){
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
void RectMinMax::addx(const double x){
    if (x < xMin) { xMin = x; }
    if (x > xMax) { xMax = x; }
}
void RectMinMax::addy(const double y){
    if (y < yMin) { yMin = y; }
    if (y > yMax) { yMax = y; }
}
void RectMinMax::add(const double x, const double y){
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
void RectMinMax::clear(const double x, const double y){
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
    return toDeg<double>(atan2(dy, dx));
}
//
//--- END - implementation of class "RectMinMax" -------------------------------
//


void RotPos90(double& x, double& y, double& width, double& height){
// Parameterübergabe als Referenz - dann werden direkt die Positionen überschrieben.
// Fallunterscheidung, in welchem Quadranten des Koordinatensystems der Punkt liegt, ist nicht nötig
    double xneu = (y + height) * (-1);
    y = x;
    x = xneu;
}

void RotPoint90(double& x, double& y){
        double tmp = y;
        y = x;
        x = (-1) * tmp;
}


/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
// Farbnamen von QET in RGB-Werte umwandeln
std::string ColorToValue (const std::string & Color){
  // hier die Liste der bekannten Farben:
  std::map<std::string, std::string> ColorMap = {
    {"white", "#FFFFFF"},
    {"black", "#000000"},
    {"green", "#00FF00"},
    {"red", "#FF0000"},
    {"blue", "#0000FF"},
    {"gray", "#A0A0A4"},
    {"grey", "#A0A0A4"},
    {"brun", "#612C00"},
    {"brown", "#612C00"},
    {"yellow", "#FFFF00"},
    {"cyan", "#00FFFF"},
    {"magenta", "#FF00FF"},
    {"lightgray", "#C0C0C0"},
    {"orange", "#FF8000"},
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
    {"HTMLGreenGreen", "#008000"},
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
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/



/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
std::string FontToFontFamily (const std::string & s){
    std::string sFontFamily;
    if      ( (!(s.find("Sans") == std::string::npos)) ||
              (!(s.find("Arial") == std::string::npos)) ||
              (!(s.find("MS Shell Dlg") == std::string::npos)) )  {
        sFontFamily = "Sans Serif,MS Sans Serif"; // "Arial", "Tahoma", "MS Sans Serif"
        }
    else if ( (!(s.find("Mono") == std::string::npos)) ||
              (!(s.find("Fixed") == std::string::npos)) ||
              (!(s.find("Courier") == std::string::npos)))  {
        sFontFamily = "Monospace,Courier New"; // "Courier", "Courier New", "Fixedsys", "Console", "Terminal"
        }
    else if ( (!(s.find("Serif") == std::string::npos)) ||
              (!(s.find("Times") == std::string::npos)))  {
        sFontFamily = "Serif,MS Serif"; // "Times", "Times New Roman", "MS Serif"
        }
    else {
        sFontFamily = "Sans Serif,MS Sans Serif"; // "Arial", "Tahoma", "MS Sans Serif"
        }
    return sFontFamily;
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/



/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
bool MultiLineText(std::string text, std::vector<std::string> & vsText){
    if ( (!(text.find("\n") == std::string::npos)) ||
         (!(text.find("\r") == std::string::npos)) )  {
        // alle "\r" durch "\n" ersetzen --> einheitliche Trennzeichen:
        std::string as[] = {"\r", "&#10;", "&#13;", "&#xa;", "&#xd;"};
        for (std::string s : as) {
            while (text.find(s) != std::string::npos ) {
                text.replace(text.find(s), s.length(), "\n");
            }
        }
        // doppelte Umbrüche wollen wir nicht!
        while (text.find("\n\n") != std::string::npos ) {
            text.replace(text.find("\n\n"), 2, "\n");
        }
        // der Text wird aufgeteilt und im Vector abgelegt:
        size_t pos;
        std::string delimiter = "\n";
        while ((pos = text.find(delimiter)) != std::string::npos) {
            vsText.push_back(text.substr(0, pos));
            text.erase(0, pos + delimiter.length());
        }
        // der letzte teil muss auch mit:
        vsText.push_back(text);
        // Ja, der übergebene Text enthält Umbrüche!
        return true;
    }
    else {
        // ohne Umbrüche kann der komplette Text in den Vector:
        vsText.push_back(text);
        return false;
    }
}
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
