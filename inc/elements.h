/*
 * Copyright (c) 2022-2025 plc-user
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

#ifndef ELEMENTS_H
#define ELEMENTS_H

#include <iostream>     // for IO-Operations
#include <cstdint>      // int8_t, ...
#include <cmath>        // sqrt, ...
#include <vector>       // für Polygone
#include <tuple>        // einzelne Punkte des Polygons
#include <map>          // die Namen des Elements

#include "pugixml/pugixml.hpp"


//
// --- function-prototypes for ... ---------------------------------------------
//
std::string ColorToValue (const std::string&);

std::string FontToFontFamily (const std::string&);

bool MultiLineText(std::string, std::vector<std::string> & );

class RectMinMax;

void RotPos90(double&, double&, double&, double&);
void RotPoint90(double&, double&);

//
// für die Verwaltung von grafischen Elementen:
//

//
//--- START - definition of definition-line of Elements ------------------------
//
class DefinitionLine {
   private:
      std::string Type = "DefinitionLine";
      std::string version = "0.100.0";
      std::string link_type = "thumbnail";
      std::string type = "element";
      int width = 10;
      int height = 12;
      int hotspot_x = 5;
      int hotspot_y = 6;
   public:
      void ReadFromPugiNode(pugi::xml_node);
      void ReCalc(RectMinMax);
      void WriteToPugiNode(pugi::xml_node);
      int Getwidth(){ return width; }
      int Getheight(){ return height; }
      int Gethotspot_x(){ return hotspot_x; }
      int Gethotspot_y(){ return hotspot_y; }
      std::string Getlink_type() { return link_type; }
      void Setwidth( const int val ){ width = val; }
      void Setheight( const int val ){ height = val; }
      void Sethotspot_x( const int val ){ hotspot_x = val; }
      void Sethotspot_y( const int val ){ hotspot_y = val; }
      void Setlinktype( const std::string val ){ link_type = val; }
   protected:
      //
};
//
//--- END - definition of definition-line of Elements --------------------------
//



//
//--- START - definition of name-list of Elements ------------------------
//
class NamesList {
    private:
      std::map<std::string, std::string> names;
      //
    public:
      void ReadFromPugiNode(pugi::xml_node);
      void WriteToPugiNode(pugi::xml_node);
      void AddName(const std::string& lang, const std::string& name) { names.insert({lang, name}); };
      void Clear(void) { names.clear(); };
      //
    protected:
      //
};
//
//--- END - definition of name-list of Elements --------------------------
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
      BaseElement(const std::string s) {
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
      bool        antialias = false;
      std::string style = "line-style:normal;line-weight:normal;filling:none;color:black";
      std::string lineStyle = "normal"; // "normal", "dotted", "dashed", "dash-dotted"
      std::string lineWeight = "normal"; // "none", "thin", "normal", "hight", "eleve"
      std::string lineFilling = "none"; // Füllfarbe
      std::string lineColor = "black"; // Zeichenfarbe
   public:
      BaseStyle() {
                   // std::cerr << " default-constructor BaseStyle\n";
                  }
      bool        GetAntialias() { return antialias; }
      std::string GetStyle() { return style; }
      double GetLineWidth() {
              if      (lineWeight == "none")
                  return 0.0;
              else if (lineWeight == "thin")
                  return 0.25;
              else if (lineWeight == "normal")
                  return 1.0;
              else if (lineWeight == "hight")
                  return 2.0;
              else if (lineWeight == "eleve")
                  return 5.0;
              else
                  return 1.0;
      }
      void SetAntialias(const bool val) { antialias = val; }
      void SetStyle(const std::string val) { style = val; }
      std::string StyleAsSVGstring(const size_t&);
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
      void Move(const double dx, const double dy) { x += dx; y += dy; }
      void Move(const double dx, const double dy, const double dz) { x += dx; y += dy; z += dz;}
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
      void SwapWidhHeight(void)        { double tmp = width;
                                         width = height;
                                         height = tmp;  }
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
      double text_width = -1.0;
      bool rotate = true;
      bool frame = false;
      bool keep_visual_rotation = false;
      std::string uuid = "";
      std::string text_from = "ElementInfo";
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
                         text_width = -1.0;
                         font = "";
                         info_name = "";
                         size = 11.1; text = "_"; color = "#000000";
                         text_from = "ElementInfo";
                         SplitFontString();
                       }
      void ReadFromPugiNode(pugi::xml_node&);
      void WriteToPugiNode(pugi::xml_node&, const size_t&);
      void SplitFontString(void);
      void CreateFontString(void);
      int GetSize(void) { return (int)round(size); }
      std::string GetText(void) { return text; }
      void Flip(void)   { y = (-1) * y; }
      void Mirror(void) { x = (-1) * x; }
      void Rot90(void);  // rotate clockwise by 90°
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x *= factX; y *= factY;
                       size *= std::min(factX, factY);
                       CreateFontString();
                       }
      std::string AsSVGstring(const size_t&);
};
//
//--- END - definition of class "ElmtDynText" ----------------------------------
//



//
//--- definition of class "ElmtText" -------------------------------------------
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
      void ReadFromPugiNode(pugi::xml_node&);
      void WriteToPugiNode(pugi::xml_node&, const size_t&);
      void SplitFontString(void);
      void CreateFontString(void);
      int GetSize(void) { return (int)round(size); }
      std::string GetText(void) { return text; }
      void Flip(void)   { y = (-1) * y; }
      void Mirror(void) { x = (-1) * x; }
      void Rot90(void);  // rotate clockwise by 90°
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x *= factX; y *= factY;
                       size *= std::min(factX, factY);
                       CreateFontString();
                       }
      std::string AsSVGstring(const size_t&);
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
      bool ReadFromPugiNode(pugi::xml_node&);
      void WriteToPugiNode(pugi::xml_node&, const size_t&);
      void InsertXat(const uint64_t, const double);
      void InsertYat(const uint64_t, const double);
      void InsertXYat(const uint64_t, const double, const double);
      void AddPoint(const double, const double);
      void AddPoint(const uint64_t, const double, const double);
      double GetMaxX() {
          double MaxX = -1e99;
          for (const auto &i : polygon)
              MaxX = std::max(MaxX, std::get<1>(i));
          return MaxX;
          }
      double GetMinX() {
          double MinX = 1e99;
          for (const auto &i : polygon)
              MinX = std::min(MinX, std::get<1>(i));
          return MinX;
          }
      double GetMaxY() {
          double MaxY = -1e99;
          for (const auto &i : polygon)
              MaxY = std::max(MaxY, std::get<2>(i));
          return MaxY;
          }
      double GetMinY() {
          double MinY = 1e99;
          for (const auto &i : polygon)
              MinY = std::min(MinY, std::get<2>(i));
          return MinY;
          }
      void Clear(){ polygon.clear(); closed = true; }
      bool CheckIndex(const std::string);
      virtual void Write(void) {
                         for(const auto &i : polygon)
                             std::cout<<std::get<0>(i)<<" - "<<std::get<1>(i)<<" - "<<std::get<2>(i)<<"\n";
                         }
      std::string AsSVGstring(const size_t&);
      void Flip(void);   // vertikal
      void Mirror(void); // horizontal
      void Rot90(void);  // rotate clockwise by 90°
      void Move(const double dx, const double dy);  // move by dx, dy
      void Scale(const double factX=1.0, const double factY=1.0);
      void CleanUp(pugi::xml_node, const double);
};
//
//--- END - definition of class "Polygon" --------------------------------------
//



//
//--- definition of class "ElmtLine" -------------------------------------------
//
class ElmtLine : public ElmtPolygon {
   protected:
       //
   private:
      double length1 = 0.0;
      double length2 = 0.0;
      std::string end1 = "none";
      std::string end2 = "none";
   public:
      ElmtLine() : ElmtPolygon("line") {
                       //std::cerr << " default-constructor ElmtLine - Typ: " << Type << "\n";
                       }
      bool ReadFromPugiNode(pugi::xml_node&);
      void WriteToPugiNode(pugi::xml_node&, const size_t&);
      double GetLength(void) {
          return (sqrt( pow((std::get<2>(polygon[1])-(std::get<2>(polygon[0]))), 2) +
                        pow((std::get<1>(polygon[1])-(std::get<1>(polygon[0]))), 2)) );
          }
      double GetAngle(void) {
          return toDeg<double>(atan2((std::get<2>(polygon[1])-(std::get<2>(polygon[0]))),
                                     (std::get<1>(polygon[1])-(std::get<1>(polygon[0]))) ) );
          }
      void SetLength1(const double val) { length1 = val; }
      void SetLength2(const double val) { length2 = val; }
      void SetEnd1(const std::string val) { end1 = val; }
      void SetEnd2(const std::string val) { end2 = val; }
      void Write(void) { ElmtPolygon::Write();
                         if (polygon.size() > 0) {
                           std::cout << length1 << " | " << length2 << "\n" << end1 << " | " << end2 << "\n";
                         }
                       }
      //void Flip(void);   // hier nix definiert: wird von übergeordneter Klasse "Polygon" übernommen!
      //void Mirror(void); // hier nix definiert: wird von übergeordneter Klasse "Polygon" übernommen!
      //void Rot90(void);  // hier nix definiert: wird von übergeordneter Klasse "Polygon" übernommen!
      void Scale(const double factX=1.0, const double factY=1.0) {
                         ElmtPolygon::Scale(factX, factY);
                         length1 *= std::min(factX, factY); length1 = std::min(length1, 99.0);
                         length2 *= std::min(factX, factY); length2 = std::min(length2, 99.0);
                       }
      std::string AsSVGstring(const size_t&);
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
      void ReadFromPugiNode(pugi::xml_node&);
      void WriteToPugiNode(pugi::xml_node&, const size_t&);
      void Flip(void)   { y = (-1) * y - height; }
      void Mirror(void) { x = (-1) * x - width; }
      void Rot90();  // rotate clockwise by 90°
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x     *= factX;      y *= factY;
                       width *= factX; height *= factY;
                       }
      std::string AsSVGstring(const size_t&);
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
        //
    private:
        double rx      = 0.0;
        double ry      = 0.0;
    public:
        ElmtRect() : BaseElement("rect") {
                       //std::cerr << " default-constructor ElmtRect - Typ: " << Type << "\n";
                       }
        void Clear(void) { BasePosition::Clear(); BaseSize::Clear(); rx = 0.0; ry = 0.0; }
        void ReadFromPugiNode(pugi::xml_node&);
        void WriteToPugiNode(pugi::xml_node&, const size_t&);
        double GetRx(void)      { return rx; }
        double GetRy(void)      { return ry; }
        void SetRx(const double val)      { rx = val; }
        void SetRy(const double val)      { ry = val; }
        void Flip(void)   { y = (-1) * y - height; }
        void Mirror(void) { x = (-1) * x - width; }
        void Rot90();  // rotate clockwise by 90°
        void Scale(const double factX=1.0, const double factY=1.0) {
                       x     *= factX;      y *= factY;
                       rx    *= factX;     ry *= factY;
                       width *= factX; height *= factY;
                       }
        std::string AsSVGstring(const size_t&);
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
       //
   private:
      double angle  = 0.0;
      double start  = 0.0;
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
      void ReadFromPugiNode(pugi::xml_node&);
      void WriteToPugiNode(pugi::xml_node&, const size_t&);
      double GetMinX() { return MinX; }
      double GetMaxX() { return MaxX; }
      double GetMinY() { return MinY; }
      double GetMaxY() { return MaxY; }
      void SetData(const double, const double, const double, const double, const double, const double);
      void SetStart(const double val) { start = val; Normalize(); }
      void SetAngle(const double val) { angle = val; Normalize(); }
      void Normalize();
      void coutData(){ std::cout << x << "|" << y << ", " << width << "|" << height << ", " << start << "|" << angle; }
      void Flip();   // vertikal
      void Mirror(); // horizontal
      void Rot90();  // rotate clockwise by 90°
      void Scale(const double factX=1.0, const double factY=1.0) {
                       x     *= factX;      y *= factY;
                       width *= factX; height *= factY;
                       DetermineMinMax();
                       }
      std::string AsSVGstring(const size_t&);
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
   protected:
       //
   private:
      std::string orientation = "n";
      std::string type        = "Generic";
      std::string name        = "";
      std::string uuid        = "{}";
   public:
      ElmtTerminal() : BaseElement("terminal") {
                       //std::cerr << " default-constructor Terminal - Typ: " << Type << "\n";
                       }
      void Clear(void) { x = 0.0; y = 0.0; orientation = "n"; type = "Generic"; name = ""; uuid = ""; }
      void ReadFromPugiNode(pugi::xml_node&);
      void WriteToPugiNode(pugi::xml_node&);
      void CreateNewUUID(void) { uuid = "{" + CreateUUID(false) + "}"; }
      std::string GetOrientation() { return orientation; }
      std::string GetType()        { return type; }
      std::string GetName()        { return name; }
      std::string GetUUID()        { return uuid; }
      void SetOrientation(const std::string val) { orientation = val; }
      void SetType(const std::string val)        { type = val; }
      void SetName(const std::string val)        { name = val; }
      void SetUUID(const std::string val)        { uuid = val; }
      std::string AsSVGstring(const size_t&);
      void Flip(void) { y *= (-1.0);
                        if (orientation == "n") orientation = "s";
                        if (orientation == "s") orientation = "n"; }   // vertikal
      void Mirror(void) { x *= (-1.0);
                        if (orientation == "e") orientation = "w";
                        if (orientation == "w") orientation = "e"; }   // horizontal
      void Rot90(void);  // rotate clockwise by 90°
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
      RectMinMax(const double,
                 const double);      // constructor with value
      RectMinMax(const RectMinMax&); // copy-constructor
      void addx(const double);       // add a new x-value
      void addy(const double);       // add a new y-value
      void add(const double,
               const double);        // add new values
      void clear(void);              // clear min/max and set to 0
      void clear(const double,
                 const double);      // clear min/max and set new values
      void clear(const RectMinMax&); // clear with value
      double xmin(void);             // returns minimum x-value
      double xmax(void);             // returns maximum x-value
      double ymin(void);             // returns minimum y-value
      double ymax(void);             // returns maximum y-value
      double width(void);            // returns (xmax - xmin)
      double height(void);           // returns (ymax - ymin)
      double diagonal(void);         // returns length of diagonal
      double angle(void);            // returns angle of diagonal
};
//
//--- END - definition of class "RectMinMax" -----------------------------------
//



#endif  //#ifndef ELEMENTS_H
