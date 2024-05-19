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

#include "helpers.h"

#include <random>       // for random values of uuid
#include <cmath>        // for sqrt, atan2, isnan(), ...
#include <iomanip>      // for IO-Operations
#include <sstream>      // for String-Streams


const char cDecSep = '.';     // Decimal-Separator for values in output-file
// for setting a (new) Decimal-Separator
struct DecSep : std::numpunct<char> {
    char do_decimal_point()   const { return cDecSep; }  // Decimal-Separator
};





/****************************************************************************/
/****************************************************************************/
/****************************************************************************/
std::string FormatValue(const double value, const size_t dec){
    if (dec == 0) {
        return std::to_string(int(std::round(value)));
    }
    double val = value;
    double epsilon = 0.1; // too small values will be set to "0"
    for (size_t i=0; i<dec; i++)
        epsilon = epsilon / 10.0;
    if (val < 0.0){    // for negative values
        if (val >= (epsilon * (-1.0)))
            val = 0.0;
    } else {           // for posivive values
        if (val <= epsilon)
            val = 0.0;
    }
    for (size_t i=0; i<dec; i++)
        val = (val * 10.0);
    long int iVal = round(val);
    // we're done, if "0"
    if (iVal == 0)
        return "0";
    // additional rounding, if value is VERY close to the next integer:
    if (dec>1) {
        int Divider = 1;
        for (size_t i=0; i<dec; i++) Divider *= 10;
        int Rest = iVal % Divider;
        if (((Rest > 0) && (Rest <  5)) || ((Rest < 0) && (Rest > -5))) {
            if (_DEBUG_) std::cerr << "Divider: " << Divider << " - Rest: " << Rest
                              << " - subtract " << Rest << std::endl;
            iVal -= Rest;
        }
        if ((Rest > 0) && (Rest > (Divider - 5))) {
            if (_DEBUG_) std::cerr << "Divider: " << Divider << " - Rest: " << Rest
                              << " - add " << (Divider - Rest) << std::endl;
            iVal += (Divider - Rest);
            }
        if ((Rest < 0) && (Rest < (-1)*(Divider - 5))) {
            if (_DEBUG_) std::cerr << "Divider: " << Divider << " - Rest: " << Rest
                              << " - subtract " << (Divider + Rest) << std::endl;
            iVal -= (Divider + Rest);
            }
    }

    // prepare to return the new value as string
    double dVal = iVal;
    for (size_t i=0; i<dec; i++)
        dVal = (dVal / 10.0);
    std::stringstream ss;
    ss.imbue(std::locale(std::cout.getloc(), new DecSep));
    ss << std::fixed << std::setprecision(dec) << dVal;
    std::string s = ss.str();  // "s" is number always (!) with decimals
    // delete this range of characters in order from the end of string:
    const char chars[] = {'0', cDecSep};
    // use range for loop to print array elements
    for (char c : chars)
        while (s[s.length()-1] == c) {
            if (_DEBUG_)
                std::cerr << "del \"" << c << "\"" << std::endl;
            s.erase(s.length()-1, 1);
        }

    return s;
}
/****************************************************************************/
/****************************************************************************/
/****************************************************************************/




//
// ###############################################################
// ###    Implementation of function "RemoveLeadingZeros"      ###
// ###############################################################
//
void RemoveLeadingZeros(std::string& s)
{   // von Strings, die eine Zahl repräsentieren, links unnötige "0" entfernen
    // eine while - Schleife scheint am sinnvollsten!
    while ( s[0] == '0' ) {
        if ( (s == "0") ||                                         // der ganze String besteht nur aus einer "0"
             ((s.length() >= 3) && (s[0] == '0') && (s[1] == '.')) ||                 // Fließkomma: 0.irgendwas
             ((s.length() >= 3) && (s[0] == '0') && ((s[1] == 'x') || (s[1] == 'X'))) // HEX: 0xIrgendwas
           ) { break; } // wir können hier schon aufhören!
         s.erase(0, 1);  // erste Stelle löschen
    } // while ...
}
//
// ###############################################################
// ###  END - Implementation of function "RemoveLeadingZeros"  ###
// ###############################################################
//



//
// ############################################################
// ###     Implementation of function "TextToEntity"        ###
// ############################################################
//
std::string TextToEntity(std::string& s){
    std::string result = "";
    for (size_t i = 0; i<s.length(); i++)
    {
        switch ((char)s[i]){
            case '&' :
            case '>' :
            case '<' :
            case '"' :
            case '\'':
                result += "&#x" + toHex<char>((char)s[i]) + ";";
                break;
            default:
                result += s[i];
        }
    }
    return result;
}
//
// ###############################################################
// ###     END - Implementation of function "TextToEntity"     ###
// ###############################################################
//



//
// ############################################################
// ###              we build a (random?) UUID               ###
// ############################################################
//
static unsigned int random_char() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 255);
    return dis(gen);
}
//
static std::string generate_hex(const unsigned int len) {
    std::string s = "";
    for (unsigned int i = 0; i < len; i++) {
        const uint8_t rc = random_char();
        s += toHex<uint8_t>(rc); // eigene Funktion ...
    }
    return s;
}
//
const std::string CreateUUID(const bool UpCase) {
    std::string uuid = "";
    uuid  =       generate_hex(4);
    uuid += '-' + generate_hex(2);
    uuid += '-' + generate_hex(2);
    uuid += '-' + generate_hex(2);
    uuid += '-' + generate_hex(6);
    for (size_t i=0; i<uuid.length(); i++)
        if (UpCase == true)
            uuid[i] = toupper(uuid[i]);
        else
            uuid[i] = tolower(uuid[i]);
    return uuid;
}
//
// ############################################################
// ###       END -  we build a (random?) UUID               ###
// ############################################################
//
