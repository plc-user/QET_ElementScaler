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

#include "helpers.h"

#include <random>       // for random values of uuid
#include <cmath>        // for sqrt, atan2, isnan(), ...
#include <iomanip>      // for IO-Operations
#include <fstream>      // for file-reading
#include <sstream>      // for String-Streams
#include <regex>        // for "double"-Check


const char cDecSep = '.';     // Decimal-Separator for values in output-file
// for setting a (new) Decimal-Separator
struct DecSep : std::numpunct<char> {
    char do_decimal_point()   const { return cDecSep; }  // Decimal-Separator
};





/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
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
/******************************************************************************/
/******************************************************************************/
/******************************************************************************/




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
// ###############################################################
// ###       Implementation of function "TextToEntity"         ###
// ###############################################################
//
std::string TextToEntity(const std::string& s){
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
// ###############################################################
// ###               we build a (random?) UUID                 ###
// ###############################################################
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
// ###############################################################
// ###         END -  we build a (random?) UUID                ###
// ###############################################################
//


//
// ###############################################################
// ###     does the string contain a double- or int-value      ###
// ###############################################################
//
void CheckForDoubleString(std::string& sArg){
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
        } else {
            // string is NOT convertable to double
            if (_DEBUG_)
                std::cerr << "can not convert \"" << sArg << "\" to float!" << std::endl;
            sArg = "WontWork";
        }
}
//
// ###############################################################
// ###   END: does the string contain a double- or int-value   ###
// ###############################################################
//




//
// ###############################################################
// ###            determine the terminal-geometry              ###
// ###############################################################
//
void get_terminal_size(size_t& width, size_t& height) {
// Quelle (mit eigenen Ergänzungen):
// https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    width = (size_t)(csbi.srWindow.Right-csbi.srWindow.Left+1);
    height = (size_t)(csbi.srWindow.Bottom-csbi.srWindow.Top+1);
#elif defined(__linux__) || defined(__APPLE__)
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    width = (size_t)(w.ws_col);
    height = (size_t)(w.ws_row);
#else
    width = (size_t)(80);
    height = (size_t)(24);
#endif // Windows/Linux
}
// ---
size_t get_terminal_height(void) {
// abgeleitet von: get_terminal_size
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return (size_t)(csbi.srWindow.Bottom-csbi.srWindow.Top+1);
#elif defined(__linux__) || defined(__APPLE__)
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    return (size_t)(w.ws_row);
#else
    return (size_t)(24);
#endif // Windows/Linux
}
// ---
size_t get_terminal_width(void) {
// abgeleitet von: get_terminal_size
#if defined(_WIN32)
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return (size_t)(csbi.srWindow.Right-csbi.srWindow.Left+1);
#elif defined(__linux__) || defined(__APPLE__)
    struct winsize w;
    ioctl(fileno(stdout), TIOCGWINSZ, &w);
    return (size_t)(w.ws_col);
#else
    return (size_t)(80);
#endif // Windows/Linux
}
//
// ###############################################################
// ###          END: determine the terminal-geometry           ###
// ###############################################################
//



//
// ###############################################################
// ###             read a piece from a text-file               ###
// ###############################################################
//
// returns a string of "length" characters around "position"
// the existence of "file" MUST be checked before!!!
std::string ReadPieceOfFile(const std::string file, size_t pos, const size_t length) {
    std::ifstream strm;
    strm.open ( file );
    strm.seekg (0, strm.end);
    if (pos < (length/2)) {
        pos = 0;
    } else if (pos > ((size_t)strm.tellg() - (length/2) )) {
        pos = (size_t)strm.tellg() - length;
    } else {
        pos -= (length/2);
    }
    strm.seekg (pos);
    std::string buffer(length, '\0');
    strm.read(&buffer[0], length);
    if (buffer.length() > length) { buffer.resize(length); }
    strm.close();
    return buffer;
}
//
// ###############################################################
// ###          END: read a piece from a text-file             ###
// ###############################################################
//


//
// ###############################################################
// ###            remove whitespace from string                ###
// ###############################################################
//
std::string trim(const std::string& source) {
    std::string s(source);
    if (source.empty())
        return s;
    // delete leading whitespace
    while (std::isspace(s[0])){
        s.erase(0,1);
    }
    // delete trailing whitespace
    while (std::isspace(s[s.length()-1])){
        s.erase(s.length()-1,1);
    }
    return s;
}
//
// ###############################################################
// ###          END: remove whitespace from string             ###
// ###############################################################
//
