/*
 * Copyright (c) 2022-2026 plc-user
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

//
// QET_ElementScaler is a commandline-tool to scale
// QElectroTech-Graphics and/or converts them to
// Scalable Vector-Graphics.
//
// usage:
// see README.md
//
// +----------------------------------------------------------+
// | This software uses pugixml library (https://pugixml.org) |
// | pugixml is Copyright (C) 2006-2026 Arseny Kapoulkine.    |
// +----------------------------------------------------------+
//

// own header(s):
#include "main.h"


// =============================================================================
// implementation
// =============================================================================

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
    if (xReadFromStdIn == true){
        result = doc.load(std::cin);
    } else {
        if (ElementFile.empty() && (argc>1)) {
            ElementFile = argv[argc-1];
        }
        if (!(ElementFile.empty()) &&
            (std::filesystem::exists(ElementFile)) &&
            (std::filesystem::is_regular_file(ElementFile))) {
            result = doc.load_file(ElementFile.c_str());
        } else {
            PrintHelp(argv[0], sVersion);
            return -1;
        }
    }
    // check the result of "doc.load"-Function
    if (!result){
        // try to read the corrupt part and output to stderr
        if (xReadFromStdIn == true) {
            std::cerr << "Data could not be loaded: " << result.description() << std::endl;
            std::cerr << "Check data up to byte-offset: " << result.offset << "\n";
        } else {
            std::cerr << "File \"" << ElementFile << "\" could not be loaded: " << result.description() << std::endl;
            std::cerr << "Check file up to byte-offset: " << result.offset << " -- content partly shown here:\n";
            std::cerr << "(...)" << ReadPieceOfFile(ElementFile, result.offset, (get_terminal_width()-10)) << "(...)\n\n";
        }
        return -1;
    } else {
        if (_DEBUG_) std::cerr << "Element-Data loaded successfully.\n";
    }

    // xml-file was successfully loaded, let's check, what kind of file/data we have...
    std::string sExt = std::filesystem::path(ElementFile).extension().string();
    std::transform(sExt.begin(), sExt.end(), sExt.begin(), ::tolower);
    if ((sExt == ".elmt") && doc.child("definition").child("description")) {
        xIsElmtFile = true;
    } else if ((ElementFile == "qet_directory") && doc.child("qet-directory").child("names")) {
        xIsDirFile  = true;
        xCreateSVG  = false; // cannot create SVG from directory-file
        xCreateELMT = true;  // use the same xml-output-function for "element" and "qet_directory"
    } else if ((sExt == ".titleblock") && doc.child("titleblocktemplate")) {
        std::cerr << "QElectroTech - titleblock-templates are not supported by QET_ElementScaler!\n";
        return -2;
    } else if ((sExt == ".qet") && doc.child("project").child("diagram") && doc.child("project").child("collection")) {
        std::cerr << "QElectroTech - project-files are not supported by QET_ElementScaler!\n";
        return -2;
    } else {
        // no file-format we can handle here -> QUIT with message
        std::cerr << "cannot handle \"" << ElementFile << "\": wrong file-content! " << std::endl;
        return -2;
    }

    // build the filename for the scaled element:
    ElementFileScaled = ElementFile;
    if (xOverwriteOriginal == true){
        std::cerr << "will overwrite original file!" << std::endl;
    } else {
        if (xPrintToStdOut == false) {
            if (xIsElmtFile == true) {
                ElementFileScaled = std::filesystem::path(ElementFile).replace_extension(".SCALED.elmt").string();
            } else if (xIsDirFile == true) {
                ElementFileScaled += ".SCALED";
            }
        }
    }
    if (_DEBUG_) std::cerr << ElementFileScaled << std::endl;

    // Process "qet_directory"
    if (xIsDirFile) {
        ProcessDirFile(doc);
    }

    // Process the Element-file: scale, flip, etc...
    if (xIsElmtFile) {
        if (xOnlyFixFonts) {
            // einige Steuerbits müssen passend gesetzt werden:
            xCreateELMT = true;
            xCreateSVG = false;
            // die Schriftarten ansehen und bei Bedarf ersetzen:
            ProcessOnlyFontsOfElement(doc);
        } else {
            ProcessElement(doc);
        }
    }

    if (xCreateSVG == true) {
      // SVG-Daten erstellen
      std::string s = ToSVG(doc);
      if (xPrintToStdOut == true) {
        // zur Standard-Ausgabe:
        std::cout << s << "\n\n";
      } else {
        // Dateinamen erstellen und SVG speichern
        SVGFile = ElementFile+".svg";
        std::ofstream out;
        out.open(SVGFile, std::ios::out);
        out << s << "\n";
        out.close();
      }
      return 0;
    }


    // save XML to a string, to be able to "edit" raw data
    std::string sXML;
    std::stringstream ssxml;
    doc.save(ssxml, "    ", pugi::format_default | pugi::format_no_declaration);
    sXML = ssxml.str();
    ssxml.clear();

    // replace some substrings so that xml looks the same as with QET
    // NO technical reason, only for the look: "You said, nothing changed, but look here..."
    ReplaceSubStrings(sXML);

    if (xCreateELMT == true) {
        if (xPrintToStdOut==true) {
            if (_DEBUG_) std::cerr << "XML auf stdout ------------------------------------------------------" << std::endl;
            std::cout << sXML;
            if (_DEBUG_) std::cerr << "XML auf stdout ------------------------------------------------------" << std::endl;
        } else {
            // If only the fonts were supposed to be replaced, but none were, we can stop here!
            if ((xOnlyFixFonts) && (uiFontsReplaced == 0)) {
                std::cerr << "No fonts replaced –> no need to save anything.\n";
                return 10;
            }
            // save string to file:
            std::ofstream outFile(ElementFileScaled);
            outFile << sXML;
            if        ((outFile.rdstate() & std::ofstream::badbit) != 0) {
                std::cerr << "file \"" << ElementFileScaled << "\" could not be saved!\n";
            } else if ((outFile.rdstate() & std::ofstream::failbit) != 0) {
                std::cerr << "saving \"" << ElementFileScaled << "\" failed!\n";
            } else {
                if (_DEBUG_) std::cerr << "file \"" << ElementFileScaled << "\" saved successfully!" << std::endl;
            }
            outFile.close();
        }
    }

  return 0;
}
