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

#ifndef _ELEMENTS_H
#define _ELEMENTS_H

#include <iostream>     // for IO-Operations
#include <cstdint>      // int8_t, ...
#include <cmath>        // sqrt, ...
#include <vector>       // für Polygone
#include <tuple>        // einzelne Punkte des Polygons

#include "pugixml/pugixml.hpp"

#define _DEBUG_ 0


//
// --- function-prototypes for ... ---------------------------------------------
//
std::string FormatValue(const double, const uint8_t);

std::string ColorToValue (const std::string);

std::string FontToFontFamily (const std::string);

double getFontSize(const std::string);

class RectMinMax;


//
// für die Verwaltung von grafischen Elementen:
//

//
//--- START - definition of definition-line of Elements ------------------------
//
class DefinitionLine {
   protected:
      // enthält nur den Element-Typ:
      std::string Type = "DefinitionLine";
      std::string version = "0.100";
      std::string link_type = "thumbnail";
      std::string type = "element";
      int width = 10;
      int height = 12;
      int hotspot_x = 5;
      int hotspot_y = 6;
   public:
      bool ReadFromPugiNode(pugi::xml_node);
      void ReCalc(RectMinMax);
      bool WriteToPugiNode(pugi::xml_node);
      int Getwidth(){ return width; }
      int Getheight(){ return height; }
      int Gethotspot_x(){ return hotspot_x; }
      int Gethotspot_y(){ return hotspot_y; }
      std::string Getlink_type() { return link_type; }
      void Setwidth( int val ){ width = val; }
      void Setheight( int val ){ height = val; }
      void Sethotspot_x( int val ){ hotspot_x = val; }
      void Sethotspot_y( int val ){ hotspot_y = val; }
      void Setlinktype( std::string val ){ link_type = val; }
      //
   private:
      //
};
//
//--- END - definition of definition-line of Elements --------------------------
//



//
//--- START - definition of Base-Classes of Elements ---------------------------
//

//
//--- definition of class "BaseElement" ----------------------------------------
//
class BaseElement {
   protected:
      // enthält nur den Element-Typ:
      std::string Type = "BaseElement";
   public:
      BaseElement(std::string s) {
                       Type = s;
                       //std::cerr << "         constructor BaseElement - Typ: " << Type << "\n";
                       }
      std::string GetType() { return Type; }
};
//
//--- END - definition of class "BaseElement" ----------------------------------
//

//
//--- definition of class "BaseStyle" ------------------------------------------
//
class BaseStyle {
   protected:
      // enthält Daten zu Linien-Stil, etc:
      std::string antialias = "false";
      std::string style = "line-style:normal;line-weight:normal;filling:none;color:black";
      std::string lineStyle = "normal"; // "normal", "dotted", "dashed", "dash-dotted"
      std::string lineWeight = "normal"; // "none", "thin", "normal", "height", "eleve"
      std::string lineFilling = "none"; // Füllfarbe
      std::string lineColor = "black"; // Zeichenfarbe
   public:
      BaseStyle() {
                   // std::cerr << " default-constructor BaseStyle\n";
                  }
      std::string GetAntialias() { return antialias; }
      std::string GetStyle() { return style; }
      void SetAntialias(const std::string val) { antialias = val; }
      void SetStyle(const std::string val) { style = val; }
      std::string StyleAsSVGstring(const uint8_t);
};
//
//--- END - definition of class "BaseStyle" ------------------------------------
//

//
//--- definition of class "BasePosition" ---------------------------------------
//
class BasePosition {
   // enthält nur die X-Y-Z-Position:
   private:
   protected:
      double x = 0.0;
      double y = 0.0;
      double z = 0.0; // vorerst nur bei dynamic_text verwendet
   public:
      BasePosition() {
                      // std::cerr << " default-constructor BasePosition\n";
                     }
      BasePosition(const double valX, const double valY) {
                   x = valX; y = valY;
                   //std::cerr << "         constructor BasePosition with XY-values\n";
                   }
      BasePosition(const double valX, const double valY, const double valZ) {
                   x = valX; y = valY; z = valZ;
                   //std::cerr << "         constructor BasePosition with xyz-values\n";
                   }
      void Clear(void) { x = 0.0; y = 0.0; z = 0.0; }
      double GetX() { return x; }
      double GetY() { return y; }
      double GetZ() { return z; }
      void SetX(const double val) { x = val; }
      void SetY(const double val) { y = val; }
      void SetZ(const double val) { z = val; }
};
//
//--- END - definition of class "BasePosition" ---------------------------------
//

//
//--- definition of class "BaseSize" -------------------------------------------
//
class BaseSize {
   // enthält nur die Breite und Höhe:
   private:
   protected:
      double width = 0.0;
      double height = 0.0;
   public:
      BaseSize() {
                  // std::cout << " default-constructor BaseSize\n";
                 }
      BaseSize(const double valX, const double valY) {
                   width = valX; height = valY;
                   //std::cout << "         constructor BaseSize with values\n";
                   }
      void Clear(void) { width = 0.0; height = 0.0; }
      double GetWidth()                { return width; }
      double GetHeight()               { return height; }
      void SetWidth(const double val)  { width = val; }
      void SetHeight(const double val) { height = val; }
};
//
//--- END - definition of class "BaseSize" -------------------------------------
//

//
//--- END - definition of Base-Classes of Elements -----------------------------
//


//
//--- BEGIN - definition of derived Classes of Elements ------------------------
//

//
//--- definition of class "ElmtDynText" ----------------------------------------
//
class ElmtDynText : public BaseElement,
                    public BasePosition
{
    private:
      bool xIsOldInput = false;
      double text_width = -1.0;
      bool rotate = true;
      bool frame = false;
      bool keep_visual_rotation = false;
      std::string uuid = "";
      std::string tagg = "none"; // old element "input"
      std::string Valignment = "AlignTop";
      std::string Halignment = "AlignLeft";
      std::vector<std::string> vsFont;
    protected:
      double size = 11.1;
      double rotation = 0.0;
      std::string color = "#000000";
      std::string text = "_";
      std::string info_name = "";
      std::string font = "Sans Serif,9,-1,5,50,0,0,0,0,0";
    public:
      ElmtDynText() : BaseElement("dynamic_text") {
                       //std::cerr << " default-constructor ElmtDynText - Typ: " << Type << "\n";
                       }
      void Clear(void) {
                         BasePosition::Clear();
                         rotate = true; rotation = 0.0;
                         xIsOldInput = false;
                         font = "";
                         tagg = "none"; info_name = "";
                         size = 11.1; text = "_"; color = "#000000";
                         SplitFontString();
                       }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node, size_t);
      void SplitFontString(void);
      void CreateFontString(void);
      int GetSize(void) { return round(size); }
      std::string GetText(void) { return text; }
      void Flip(void)   { y = (-1) * y; }
      void Mirror(void) { x = (-1) * x; }
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x *= factX; y *= factY;
                       size *= std::min(factX, factY);
                       CreateFontString();
                       }
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
};
//
//--- END - definition of class "ElmtDynText" ----------------------------------
//



//
//--- definition of class "ElmtText" -----------------------------------------
//
class ElmtText : public BaseElement,
                 public BasePosition
{  // Schrift-Eigenschaften (Größe, etc.) extrahieren bzw. bearbeiten
   private:
      std::vector<std::string> vsFont;
   protected:
      double size = 11.1;
      double rotation = 0.0;
      std::string color = "#000000";
      std::string text = "_";
      std::string font = "Sans Serif,9,-1,5,50,0,0,0,0,0";
                      //  |          | |  | |  | | | | |
                      //  |          | |  | |  | | | | +---  ???
                      //  |          | |  | |  | | | +-----  ???
                      //  |          | |  | |  | | +-------  1 -> durchgestrichen
                      //  |          | |  | |  | +---------  1 -> unterstrichen
                      //  |          | |  | |  +-----------  1 -> kursiv
                      //  |          | |  | +--------------  ???
                      //  |          | |  +----------------  ???
                      //  |          | +-------------------  ???
                      //  |          +---------------------  Schriftgröße
                      //  +--------------------------------  Schriftart
                      // "MS Shell Dlg 2,9,-1,5,50,0,0,0,0,0,Normál"
                      //  |              | |  | |  | | | | | |
                      //  |              | |  | |  | | | | | +----  (Beschreibung Stil?)
                      //  |              | |  | |  | | | | +------  ???
                      //  |              | |  | |  | | | +--------  ???
                      //  |              | |  | |  | | +----------  1 -> durchgestrichen
                      //  |              | |  | |  | +------------  1 -> unterstrichen
                      //  |              | |  | |  +--------------  1 -> kursiv
                      //  |              | |  | +-----------------  ???
                      //  |              | |  +-------------------  ???
                      //  |              | +----------------------  ???
                      //  |              +------------------------  Schriftgröße
                      //  +---------------------------------------  Schriftart
   public:
      ElmtText() : BaseElement("text") {
                       //std::cerr << " default-constructor ElmtText - Typ: " << Type << "\n";
                       }
      void Clear(void) {
                         BasePosition::Clear(); rotation = 0.0;
                         font = "";
                         size = 11.1; text = "_"; color = "#000000";
                         SplitFontString();
                       }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node, size_t);
      void SplitFontString(void);
      void CreateFontString(void);
      int GetSize(void) { return round(size); }
      std::string GetText(void) { return text; }
      void Flip(void)   { y = (-1) * y; }
      void Mirror(void) { x = (-1) * x; }
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x *= factX; y *= factY;
                       size *= std::min(factX, factY);
                       CreateFontString();
                       }
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
};
//
//--- END - definition of class "ElmtText" -------------------------------------
//



//
//--- definition of class "ElmtPolygon" ----------------------------------------
//
class ElmtPolygon : public BaseElement,
                    public BaseStyle
{
   protected:
      std::vector<std::tuple<uint64_t, double, double>> polygon;
      bool closed = true;
   private:

   public:
      ElmtPolygon() : BaseElement("polygon") {
                       //std::cerr << " default-constructor ElmtPolygon - Typ: " << Type << "\n";
                       }
      ElmtPolygon(std::string s) : BaseElement(s) {
                       //std::cerr << "         constructor ElmtPolygon - Typ: " << Type << "\n";
                       }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node, size_t);
      void InsertXat(uint64_t, double);
      void InsertYat(uint64_t, double);
      void InsertXYat(uint64_t, double, double);
      void AddPoint(double, double);
      void AddPoint(uint64_t, double, double);
      double GetMaxX() {
          double MaxX = -1e99;
          for (const auto &i : polygon)
              MaxX = std::max(MaxX, std::get<1>(i));
          return MaxX;
          };
      double GetMinX() {
          double MinX = 1e99;
          for (const auto &i : polygon)
              MinX = std::min(MinX, std::get<1>(i));
          return MinX;
          };
      double GetMaxY() {
          double MaxY = -1e99;
          for (const auto &i : polygon)
              MaxY = std::max(MaxY, std::get<2>(i));
          return MaxY;
          };
      double GetMinY() {
          double MinY = 1e99;
          for (const auto &i : polygon)
              MinY = std::min(MinY, std::get<2>(i));
          return MinY;
          };
      void Clear(){ polygon.clear(); closed = true; }
      bool CheckIndex(void);
      virtual void Write(void) {
                         for(const auto &i : polygon)
                             std::cout<<std::get<0>(i)<<" - "<<std::get<1>(i)<<" - "<<std::get<2>(i)<<"\n";
                         };
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
      void Flip(void);   // vertikal
      void Mirror(void); // horizontal
      void Scale(const double factX=1.0, const double factY=1.0);
};
//
//--- END - definition of class "Polygon" --------------------------------------
//



//
//--- definition of class "ElmtLine" -------------------------------------------
//
class ElmtLine : public ElmtPolygon {
   protected:
      double length1 = 1.5;
      double length2 = 1.5;
      std::string end1 = "none";
      std::string end2 = "none";
   private:

   public:
      ElmtLine() : ElmtPolygon("line") {
                       //std::cerr << " default-constructor ElmtLine - Typ: " << Type << "\n";
                       }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node, size_t);
      double GetLength(void) {
          return (sqrt( pow((std::get<2>(polygon[1])-(std::get<2>(polygon[0]))), 2) +
                        pow((std::get<1>(polygon[1])-(std::get<1>(polygon[0]))), 2)) );
          }
      int GetAngle(void) {
          const double pi = 3.14159265359;
          return (int)(round(atan2((std::get<2>(polygon[1])-(std::get<2>(polygon[0]))),
                                   (std::get<1>(polygon[1])-(std::get<1>(polygon[0])))) * 180 / pi)) % 360;
          }
      void SetLength1(const double val) { length1 = val; }
      void SetLength2(const double val) { length2 = val; }
      void SetEnd1(const std::string val) { end1 = val; }
      void SetEnd2(const std::string val) { end2 = val; }
      void Write(void) { ElmtPolygon::Write();
                         if (polygon.size() > 0) {
                           std::cout << length1 << " | " << length2 << "\n" << end1 << " | " << end2 << "\n";
                         }
                       };
      //void Flip(void);   // hier nix definiert: wird von übergeordneter Klasse "Polygon" übernommen!
      //void Mirror(void); // hier nix definiert: wird von übergeordneter Klasse "Polygon" übernommen!
      void Scale(const double factX=1.0, const double factY=1.0) {
                         ElmtPolygon::Scale(factX, factY);
                         length1 *= std::min(factX, factY);
                         length2 *= std::min(factX, factY);
                       };
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
};
//
//--- END - definition of class "ElmtLine" -------------------------------------
//



//
//--- definition of class "ElmtEllipse" ----------------------------------------
//
class ElmtEllipse : public BaseElement,
                    public BasePosition,
                    public BaseSize,
                    public BaseStyle
{
   protected:
      //
   private:
      //
   public:
      ElmtEllipse() : BaseElement("ellipse") {
                       //std::cerr << " default-constructor ElmtEllipse - Typ: " << Type << "\n";
                       }
      void Clear(void) { BasePosition::Clear(); BaseSize::Clear(); }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node, size_t);
      void Flip(void)   { y = (-1) * y - height; }
      void Mirror(void) { x = (-1) * x - width; }
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x     *= factX;      y *= factY;
                       width *= factX; height *= factY;
                       }
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
};
//
//--- END - definition of class "ElmtEllipse" ----------------------------------
//



//
//--- definition of class "ElmtRect" -------------------------------------------
//
class ElmtRect : public BaseElement,
                 public BasePosition,
                 public BaseSize,
                 public BaseStyle
{
   protected:
      double rx      = 0.0;
      double ry      = 0.0;
   private:

   public:
      ElmtRect() : BaseElement("rect") {
                       //std::cerr << " default-constructor ElmtRect - Typ: " << Type << "\n";
                       }
      void Clear(void) { BasePosition::Clear(); BaseSize::Clear(); rx = 0.0; ry = 0.0; }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node, size_t);
      double GetRx(void)      { return rx; }
      double GetRy(void)      { return ry; }
      void SetRx(const double val)      { rx = val; }
      void SetRy(const double val)      { ry = val; }
      void Flip(void)   { y = (-1) * y - height; }
      void Mirror(void) { x = (-1) * x - width; }
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x     *= factX;      y *= factY;
                       rx    *= factX;     ry *= factY;
                       width *= factX; height *= factY;
                       }
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
};
//
//--- END - definition of class "ElmtRect" -------------------------------------
//



//
//--- definition of class "ElmtArc" --------------------------------------------
//
class ElmtArc : public BaseElement,
                public BasePosition,
                public BaseSize,
                public BaseStyle
{
   protected:
      double angle  = 0.0;
      double start  = 0.0;
   private:
      double MinX =  1e99;
      double MaxX = -1e99;
      double MinY =  1e99;
      double MaxY = -1e99;
      void DetermineMinMax();
      //
   public:
      ElmtArc() : BaseElement("arc") {
                       //std::cerr << " default-constructor ElmtArc - Typ: " << Type << "\n";
                       }
      //
      double GetStart()  { return start; }
      double GetAngle()  { return angle; }
      //
      void Clear(void) { BasePosition::Clear(); BaseSize::Clear(); start = 0.0; angle = 0.0; }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node, size_t);
      double GetMinX() { return MinX; }
      double GetMaxX() { return MaxX; }
      double GetMinY() { return MinY; }
      double GetMaxY() { return MaxY; }
      void SetData(double, double, double, double, double, double);
      void SetStart(const double val) { start = val; Normalize(); }
      void SetAngle(const double val) { angle = val; Normalize(); }
      void Normalize();
      void coutData(){ std::cout << x << "|" << y << ", " << width << "|" << height << ", " << start << "|" << angle; }
      void Flip();   // vertikal
      void Mirror(); // horizontal
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x     *= factX;      y *= factY;
                       width *= factX; height *= factY;
                       DetermineMinMax();
                       }
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
};
//
//--- END - definition of class "ElmtArc" --------------------------------------
//



//
//--- definition of class "ElmtTerminal" ---------------------------------------
//
class ElmtTerminal : public BaseElement,
                     public BasePosition
{
   // fügt ... hinzu
   private:
   protected:
      std::string orientation = "n";
      std::string type        = "Generic";
      std::string name        = "";
      std::string uuid        = "{}";
   public:
      ElmtTerminal() : BaseElement("terminal") {
                       //std::cerr << " default-constructor Terminal - Typ: " << Type << "\n";
                       }
      void Clear(void) { x = 0.0; y = 0.0; orientation = "n"; type = "Generic"; name = ""; uuid = ""; }
      bool ReadFromPugiNode(pugi::xml_node);
      bool WriteToPugiNode(pugi::xml_node);
      void CreateNewUUID(void) { uuid = "{" + CreateUUID(false) + "}"; }
      std::string GetOrientation() { return orientation; }
      std::string GetType()        { return type; }
      std::string GetName()        { return name; }
      std::string GetUUID()        { return uuid; }
      void SetOrientation(const std::string val) { orientation = val; }
      void SetType(const std::string val)        { type = val; }
      void SetName(const std::string val)        { name = val; }
      void SetUUID(const std::string val)        { uuid = val; }
      std::string AsXMLstring(const uint8_t);
      std::string AsSVGstring(const uint8_t);
      void Flip(void) { y *= (-1.0);
                        if (orientation == "n") orientation = "s";
                        if (orientation == "s") orientation = "n"; }   // vertikal
      void Mirror(void) { x *= (-1.0);
                        if (orientation == "e") orientation = "w";
                        if (orientation == "w") orientation = "e"; }   // horizontal
      void Scale(const double factX=1.0, const double factY=1.0){
                        x *= factX; y *= factY;
                        }
};
//
//--- END - definition of class "ElmtTerminal" ---------------------------------
//


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
// for outputting the whole bunch:
inline std::ostream& operator << (std::ostream& strm, const RectMinMax& r)
{
	strm << "x:(" << r.xMin << " ... " << r.xMax << ")"
         << " | "
         << "y:(" << r.yMin << " ... " << r.yMax << ")";
	return strm;
}
//
//--- END - definition of class "RectMinMax" -----------------------------------
//



#endif  //#ifndef _ELEMENTS_H
