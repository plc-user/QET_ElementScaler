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

//
// QET_ElementScaler is a commandline-tool to scale
// QElectroTech-Graphics and/or converts them to
// Scalable Vector-Graphics.
//
// usage:
// QET_ElementScaler [options] <file>
//
// compiles with C++17 enabled on
// - Debian/GNU Linux (unstable)
// - ReactOS (0.4.15-dev-6811)
// - win
// - ???
// (see compile.sh and/or compile.cmd)
//
// Change(s) for 0.5.0beta1
// - most of the source code has been rewritten
// - added ability to export SVG-data
// - adjusted order of commandline-parameters: see --help
//
// +----------------------------------------------------------+
// | This software uses pugixml library (https://pugixml.org) |
// | pugixml is Copyright (C) 2006-2023 Arseny Kapoulkine.    |
// +----------------------------------------------------------+
//

// own headers:
#include "inc/pugixml/pugixml.hpp"
#include "inc/helpers.h"
#include "inc/elements.h"
#include "main.h"


// ============================================================================
// implementation
// ============================================================================

int main(int argc, char **argv)
{
    // read and check the Commandline-Parameters
    int iRetVal = parseCommandline(argc, argv);
    if (_DEBUG_) std::cerr << "iRetVal: " << iRetVal << std::endl;

    if (xPrintHelp == true) {
        PrintHelp(argv[0], sVersion);
        return 0;
    }

    if (xStopWithError == true) {
        std::cerr << "check options and try again!" << std::endl;
        return -1;
    }

    // load Element from stdin or XML-File
    pugi::xml_document doc;
    pugi::xml_parse_result result;
    if (xReadFromStdIn){
        result = doc.load(std::cin);
    } else {
        if ((ElementFile != "") && (std::filesystem::exists(ElementFile))) {
          //ElementFile = argv[1];
          result = doc.load_file(ElementFile.c_str());
        } else
        if ((argc>1)&&(std::filesystem::exists(argv[argc-1]))) {
          ElementFile = argv[argc-1];
          result = doc.load_file(ElementFile.c_str());
        } else
        {
            PrintHelp(argv[0], sVersion);
            return -1;
        }
    }
    // check the result of "doc.load"-Function
    if (!result){
        std::cerr << "file \"" << ElementFile << "\" could not be loaded: " << result.description() << std::endl;
        return -1;
    } else {
        if (_DEBUG_) std::cerr << "Element-File loaded successfully.\n";
    }


    // build the filename for the scaled element:
    ElementFileScaled = ElementFile;
    if (xOverwriteOriginal == true){
        std::cerr << "will overwrite original file!" << std::endl;
    } else {
        ElementFileScaled.insert(ElementFileScaled.length()-5, ".SCALED");
    }
    if (_DEBUG_) std::cerr << ElementFileScaled << std::endl;


    // Process the Element-file: scale, flip, etc...
    ProcessElement(doc);


    if (xCreateSVG == true) {
      // SVG-Daten erstellen
      std::string s = ToSVG(doc);
      if (xPrintToStdOut == true) {
        // zur Standard-Ausgabe:
        std::cout << s << "\n\n";
      }
      else {
        // Dateinamen erstellen und SVG speichern
        SVGFile = ElementFile+".svg";
        std::ofstream out;
        out.open(SVGFile, std::ios::out);
        out << s << "\n";
        out.close();
      }
      return 0;
    }


    if (xCreateELMT == true) {
        if (xPrintToStdOut==true){
            if (_DEBUG_) std::cerr << "XML auf stdout ------------------------------------------------------" << std::endl;
            doc.save(std::cout, "    ", pugi::format_default | pugi::format_no_declaration);
            // no result from "doc.save" when sending XML to cout ...
            if (_DEBUG_) std::cerr << "XML auf stdout ------------------------------------------------------" << std::endl;
            return 0;
        } else {
            // we try to save the new XML:
            if (doc.save_file(ElementFileScaled.c_str(), "    ", pugi::format_default | pugi::format_no_declaration) == true){
                if (_DEBUG_) std::cerr << "file \"" << ElementFileScaled << "\" saved successfully!" << std::endl;
                return 0;
            } else {
                std::cerr << "file \"" << ElementFileScaled << "\" could not be saved!" << std::endl;
                return -1;
            }
        }
    }

  return 0;
}
