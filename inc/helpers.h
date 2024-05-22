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

#ifndef HELPERS_H
#define HELPERS_H

#include <iostream>     // for IO-Operations
#include <cstdint>      // int8_t, ...

#define _DEBUG_ 0


//
// --- function-prototype for formatted double-to-string -----------------------
//
std::string FormatValue(const double, const size_t);

//
// --- function-prototype for UUID-calculation ---------------------------------
//
const std::string CreateUUID(bool UpCase=false);

//
// --- function-prototype for removing leading zeros ---------------------------
//
// wie der Name sagt: von Strings, die Zahlen darstellen,
// werden unnütze führende Nullen entfernt. Es wird direkt
// der String bearbeitet (Übergabe als Referenz)
void RemoveLeadingZeros(std::string& s);

//
// --- function-prototype for converting text to text with HTML-entities -------
//
std::string TextToEntity(const std::string& s);


//
// --- does the string contain a double- or int-value? -------------------------
//
void CheckForDoubleString(std::string& s);


//
// Funktionen, die vorher auch schon da waren
//

//
// eine allgemeingültige Funktion für "int2hex" und "float2hex":
//
template <typename T>
const std::string toHex(const T inVal, const bool upCase=false, const bool withPrefix=false)
// beim Aufruf muss (!) der Datentyp angegeben werden:
// toHex<int32_t>(Int32Val)
{
    static const char tab[] = "0123456789abcdef";
    static const char TAB[] = "0123456789ABCDEF";
    uint64_t val = 0;
    // hier die Unterscheidung je nach Eingabe-Datentyp:
    if      (std::is_same<T, float>::value)  {
                  const uint32_t *fval = (uint32_t*)(&inVal);
                  val = *fval;
                  }
    else if (std::is_same<T, double>::value) {
                  const uint64_t *fval = (uint64_t*)(&inVal);
                  val = *fval;
                  }
    else
        switch (sizeof(inVal)){
            case 1: val = (uint64_t)((uint8_t)  inVal);
                    break;
            case 2: val = (uint64_t)((uint16_t) inVal);
                    break;
            case 4: val = (uint64_t)((uint32_t) inVal);
                    break;
            case 8: val =           ((uint64_t) inVal);
                    break;
            default: return "";
        }
    // ab hier wird der Wert in HEX ausgegeben:
    std::string s = "";
    for (int8_t i=(2*sizeof(inVal)-1); i>=0; i--) {
        if (upCase == true)
            { s += TAB[(val >> (4*i)) & static_cast<uint16_t>(0x0f)]; }
        else
            { s += tab[(val >> (4*i)) & static_cast<uint16_t>(0x0f)]; }
        }
    return (withPrefix ? "0x" : "") + s;
}


#endif  //#ifndef HELPERS_H
