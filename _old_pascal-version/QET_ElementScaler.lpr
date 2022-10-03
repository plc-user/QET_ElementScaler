program QET_ElementScaler;

//
// QET_ElementScaler is a commandline-tool to scale
// QElectroTech-Graphics with constant factor(s).
//
// compiles with Lazarus 2.2.2 and FreePascal 3.2.2 on
// Debian/GNU Linux (unstable) and ReactOS (0.4.15-dev-4888)
//
// usage:
// QET_ElementScaler [-i] [-o] <file> <scaling-factor>
//
// OR
// QET_ElementScaler [-i] [-o] [-x FactorForX] [-y FactorForY] -f FILENAME
//
//
// Result is a XML-File with an additional header-line
// <?xml version="1.0" encoding="utf-8"?>
// Use tools from your system to remove this line for
// productive use of the scaled element in QET.
// On ReactOS (or win) "more" is available:
//   more +1 < "filename.elmt" > "filename_new.elmt"
// On Linux you may use something like this:
//   grep -v -i "xml version" "filename.elmt" > "filename_new.elmt"
//
// Last Change(s): 14.08.2022
// - add option "-i" and "--stdin" to read element-data from stdin instead
//   of a file. With this option a filename is not necessary and will be
//   ignored and the output is forced to stdout
// - renamed option "-s" to "-o" and added "--stdout" for writing to stdout
//
// Created: 17.12.2020
// Author: plc-user
// https://github.com/plc-user
//
// based on example-code from
// https://wiki.freepascal.org/XML_Tutorial
//
// Use this Software at your own risk!
//


{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Classes, SysUtils, IOStream, CustApp,
  DOM, XMLRead, XMLWrite;

type

  { TQETScaler }

  TQETScaler = class(TCustomApplication)
  protected
    procedure DoRun; override;
  public
    constructor Create(TheOwner: TComponent); override;
    destructor Destroy; override;
    procedure WriteHelp; virtual;
  end;

{ TQETScaler }

const
  sVersion: string = '0.3beta1';

var
  QETfile: TXMLDocument;    // holds the XML-Data from/to file
  gfFactor:  double = 1.0;  // Scaling-Factor for circles and Text-size
  gfFactorX: double = 1.0;  // Scaling-Factor X-direction
  gfFactorY: double = 1.0;  // Scaling-Factor Y-direction
  cDecSepOrg: char;         // voreingestellter Dezimal-Trenner
  cDecSepNew: char = '.';   // neuer Dezimal-Trenner

// Funktionen außerhalb der Class .............


function DelTrailingZeros(s: string):string;
begin
  // nur Fließkomma-Zahlen...
  if (Pos (cDecSepNew, s) = 0) then begin
    result := s;
    exit;
  end;
  // Nullen machen am Ende keinen Sinn
  while (s[length(s)] = '0') do
    delete(s, length(s), 1);
  // Dezimaltrenner ist am Ende auch nutzlos
  if (s[length(s)] = cDecSepNew) then
    delete(s, length(s), 1);
  // das war's schon
  result := s;
end;


function FaktorAnwenden(elem, name: unicodestring; val: string): string;
// multiplies the Value "s" with factor and
// returns a value with max. 2 decimals
// removes trailing zeros
var
  fNewVal: double = 0.0;
  uuidNEW: TGuid;
  slFont: TStringList;
  iNewFontSize: longint;
begin
  if ((elem = 'text') or (elem = 'dynamic_text')) and (name = 'font') then begin
    slFont := TStringList.Create;
    slFont.AddDelimitedText(val,',',true);
    iNewFontSize := round(slFont[1].ToDouble * gfFactor); // only whole numbers!
    slFont[1] := iNewFontSize.ToString;
    val := slFont.CommaText;
    val := StringReplace(val, '"' , '',[rfReplaceAll]);
    end;
  if ((elem = 'circle') and (name = 'diameter')) or
     ((elem = 'text') and (name = 'size'))
    then begin
      fNewVal := (strtofloat(val) * gfFactor);
      if ((elem = 'text') and (name = 'size'))
        then fNewVal := round (fNewVal); // only whole numbers for Text-size!
      result := (Format('%0.2f', [fNewVal]));
      if (elem = 'definition') then begin
        result := inttostr(round(fNewVal));
        exit;
        end;
      result := DelTrailingZeros(result);
    end
  else if ((elem = 'uuid') and (name = 'uuid')) then begin
    if (CreateGUID(uuidNEW) = 0) then
      result := LowerCase(GUIDToString(uuidNEW));
    end
  else result := val;
end;


function FaktorXAnwenden(elem, name: unicodestring; val: string): string;
// multiplies the Value "s" with factor and
// returns a value with max. 2 decimals
// removes trailing zeros
var
  fNewVal: double = 0.0;
  uuidNEW: TGuid;
begin
  if ((elem = 'polygon') and (name[1] = 'x')) or
     ((elem = 'definition') and ((name = 'width') or (name = 'hotspot_x'))) or
     ((elem = 'rect') and ((name = 'width') or (name = 'x') or (name = 'rx'))) or
     ((elem = 'line') and ((name = 'x1') or (name = 'x2'))) or
     ((elem = 'dynamic_text') and ((name = 'x'))) or
     ((elem = 'text') and (name = 'x')) or
     ((elem = 'input') and (name = 'x')) or
     ((elem = 'terminal') and (name = 'x')) or
     ((elem = 'arc') and ((name = 'width') or (name = 'x'))) or
     ((elem = 'circle') and (name = 'x')) or
     ((elem = 'ellipse') and ((name = 'x') or (name = 'width')))
    then begin
      fNewVal := (strtofloat(val) * gfFactorX);
      result := (Format('%0.2f', [fNewVal]));
      if (elem = 'definition') then begin
        result := inttostr(round(fNewVal));
        exit;
        end;
      result := DelTrailingZeros(result);
    end
  else if ((elem = 'uuid') and (name = 'uuid')) then begin
    if (CreateGUID(uuidNEW) = 0) then
      result := LowerCase(GUIDToString(uuidNEW));
    end
  else result := val;
end;


function FaktorYAnwenden(elem, name: unicodestring; val: string): string;
// multiplies the Value "s" with factor and
// returns a value with max. 2 decimals
// removes trailing zeros
var
  fNewVal: double = 0.0;
  uuidNEW: TGuid;
begin
  if ((elem = 'polygon') and (name[1] = 'y')) or
     ((elem = 'definition') and ((name = 'height') or (name = 'hotspot_y'))) or
     ((elem = 'rect') and ((name = 'height') or (name = 'y') or (name = 'ry'))) or
     ((elem = 'line') and ((name = 'y1') or (name = 'y2'))) or
     ((elem = 'dynamic_text') and (name = 'y')) or
     ((elem = 'text') and (name = 'y')) or
     ((elem = 'input') and (name = 'y')) or
     ((elem = 'terminal') and (name = 'y')) or
     ((elem = 'arc') and ((name = 'height') or (name = 'y'))) or
     ((elem = 'circle') and (name = 'y')) or
     ((elem = 'ellipse') and ((name = 'y') or (name = 'height')))
    then begin
      fNewVal := (strtofloat(val) * gfFactorY);
      result := (Format('%0.2f', [fNewVal]));
      if (elem = 'definition') then begin
        result := inttostr(round(fNewVal));
        exit;
        end;
      result := DelTrailingZeros(result);
    end
  else if ((elem = 'uuid') and (name = 'uuid')) then begin
    if (CreateGUID(uuidNEW) = 0) then
      result := LowerCase(GUIDToString(uuidNEW));
    end
  else result := val;
end;


procedure ScaleGraphics;
var
  iNode: TDOMNode;
  i: integer = 0;

  procedure ProcessNode(Node: TDOMNode);
  var
    cNode: TDOMNode;
    i: integer = 0;
  begin
    if Node = nil then Exit; // Stops if reached a leaf
    if Node.HasAttributes then
      begin
        // hier die Attribute durchgehen und alle Maß-Angaben ändern
        for i := 0 to (Node.Attributes.Length-1) do begin
          Node.Attributes[i].NodeValue := unicodestring(FaktorAnwenden(Node.NodeName,
                                                         Node.Attributes[i].NodeName,
                                                         string(Node.Attributes[i].NodeValue)));
          Node.Attributes[i].NodeValue := unicodestring(FaktorXAnwenden(Node.NodeName,
                                                         Node.Attributes[i].NodeName,
                                                         string(Node.Attributes[i].NodeValue)));
          Node.Attributes[i].NodeValue := unicodestring(FaktorYAnwenden(Node.NodeName,
                                                         Node.Attributes[i].NodeName,
                                                         string(Node.Attributes[i].NodeValue)));
          end;
      end;
    // zum Kindknoten weitergehen
    cNode := Node.ChildNodes.Item[0];
    // Alle Kindknoten bearbeiten
    while cNode <> nil do
    begin
      ProcessNode(cNode);
      cNode := cNode.NextSibling;
    end;
  end;

begin
  iNode := QETfile.DocumentElement.ChildNodes.Item[0];
  if iNode.ParentNode.HasAttributes then begin
    for i := 0 to (iNode.ParentNode.Attributes.Length-1) do begin
      iNode.ParentNode.Attributes[i].NodeValue := unicodestring(FaktorAnwenden(iNode.ParentNode.NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeName,
                                                               string(iNode.ParentNode.Attributes[i].NodeValue)));
      iNode.ParentNode.Attributes[i].NodeValue := unicodestring(FaktorXAnwenden(iNode.ParentNode.NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeName,
                                                               string(iNode.ParentNode.Attributes[i].NodeValue)));
      iNode.ParentNode.Attributes[i].NodeValue := unicodestring(FaktorYAnwenden(iNode.ParentNode.NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeName,
                                                               string(iNode.ParentNode.Attributes[i].NodeValue)));
      end;
    end;
  while iNode <> nil do
  begin
    ProcessNode(iNode); // Rekursiv
    iNode := iNode.NextSibling;
  end;
end;


// "Main" - Program

procedure TQETScaler.DoRun;
var
  sDateiName: string = '';
  ErrorMsg: string;
  xExtMode: boolean = false;
  xUseStdIO: boolean = false;
  ios: TIOStream; // Lesen von Daten aus stdin
  ssInputXML:  TStringStream; // für Daten aus stdin
  ssScaledXML: TStringStream; // für Daten nach stdout
begin
  // Remember Setting for Decimal-Separator:
  cDecSepOrg := DefaultFormatSettings.DecimalSeparator;
  DefaultFormatSettings.DecimalSeparator := cDecSepNew;

  // Schnelltest der Parameter
  ErrorMsg:=CheckOptions('hiox:y:f:v', 'help stdin stdout file: version');
  if ErrorMsg<>'' then begin
    ShowException(Exception.Create(ErrorMsg));
    writeln(' * * * STOP * * *');
    Terminate;
    Exit;
  end;

  // ein paar Flags für die Programmsteuerung setzen
  xUseStdIO := hasOption('i', 'stdin');  // wir holen die Daten von StdIn?
  xExtMode  := hasOption('x') or hasOption('y');

  // Version ausgeben und beenden
  if HasOption('v', 'version') then begin
    writeln;
    writeln(ExtractFileName(ExeName), ' version ', sVersion);
    writeln;
    Terminate;
    Exit;
  end;

  // da braucht jemand Hilfe
  if HasOption('h', 'help') then begin
    WriteHelp;
    writeln(' - - - no file read / changed - - -');
    Terminate;
    Exit;
  end;

  // weniger als 2 Parameter gehen sonst gar nicht!
  if (ParamCount < 2)
    then begin
      WriteHelp;
      writeln(' - - - no file read / changed - - -');
      Terminate;
      Exit;
    end;

  // Faktor für X
  if hasOption('x') then
  begin
    try
      gfFactorX := getOptionValue('x').ToDouble;
    except
      writeln('Not a valid number for x: ',getOptionValue('x'));
      writeln(' * * * STOP * * *');
      Terminate;
      exit;
    end;
  end;

  // Faktor für Y
  if hasOption('y') then
  begin
    try
      gfFactorY := getOptionValue('y').ToDouble;
    except
      writeln('Not a valid number for y: ',getOptionValue('y'));
      writeln(' * * * STOP * * *');
      Terminate;
      exit;
    end;
  end;

  // die Datei zum Bearbeiten
  if hasOption('f', 'file') then
  begin
    try
      sDateiName := getOptionValue('f', 'file');
    except
      writeln('invalid file: ', sDateiName);
      writeln(' * * * STOP * * *');
      Terminate;
      exit;
    end;
  end;

// den ursprünglichen Modus versuchen:
  if (xExtMode = false)
    then begin
      if (xUseStdIO = false)
        then begin // nur, wenn Datei gelesen werden soll
          // gibt es die Datei überhaupt?
          sDateiName := ParamStr(ParamCount - 1);
          if (fileExists(sDateiName) = false) then
            begin
              writeln('File not found: ', sDateiName);
              writeln(' - - - no file read / changed - - -');
              Terminate;
              exit;
            end;
        end;
      try  // Skalierungsfaktor als letzter Parameter
        gfFactor := ParamStr(ParamCount).ToDouble;
        gfFactorX := gfFactor;
        gfFactorY := gfFactor;
      except
        writeln('Not a valid number for factor: ',ParamStr(2));
        WriteHelp;  // Help needed!
        writeln(' - - - no file read / changed - - -');
        Terminate;
        exit;
      end;
    end;


// hier geht das eigentliche Programm los!

  // bevor es losgeht, den Dateinamen und Existenz prüfen:
  if (xUseStdIO = false) and (ExtractFileExt(sDateiName.ToLower) <> '.elmt')
    then begin
      writeln('invalid file-type: ', sDateiName);
      writeln(' * * * STOP * * *');
      Terminate;
      exit;
    end;
  if (xUseStdIO = false) and not(FileExists(sDateiName))
    then begin
      writeln('File not found: ', sDateiName);
      writeln(' * * * STOP * * *');
      Terminate;
      exit;
    end;

  // Read data to structure:
  if xUseStdIO // ohne Eingabedatei keine Ausgabedatei!
    then begin
      try  // ... to read from StdIn
        ios := TIOStream.Create(iosInput);
        ssInputXML := TStringStream.Create('');
        ssInputXML.CopyFrom(ios, 0);
      finally
        ios.Free;
      end;
      if (ssInputXML.Size > 0)
        then begin
          ssInputXML.Position := 0;
          ReadXMLFile(QETfile, ssInputXML);
          ssInputXML.Free;
          end
        else begin
          writeln('No Data from StdIn ');
          writeln(' * * * STOP * * *');
          Terminate;
          exit;
          end;
      end
    else begin  // read Data from file
      ReadXMLFile(QETfile, sDateiName);
      end;



  // process data-conversion:
  if (gfFactorX < gfFactorY)
    then gfFactor := gfFactorX
    else gfFactor := gfFactorY;
  ScaleGraphics();

  // write changed data to stdout or new file
  // force output to stdout, when input comes from stdin!
  if hasOption('o', 'stdout') or (xUseStdIO = true)
    then
      begin // write new data to stdout
        try
          ssScaledXML := TStringStream.create('');
          WriteXMLFile(QETfile, ssScaledXML);
          writeln(ssScaledXML.DataString);
        finally
          ssScaledXML.Free; // Speicher wieder freigeben
        end;
      end
    else  // write new data to renamed file
      begin
        WriteXMLFile(QETfile, changeFileExt(sDateiName, '.SCALED.elmt'));
      end;


  // Einstellungen wieder auf Original:
  DefaultFormatSettings.DecimalSeparator := cDecSepOrg;

  // stop program loop
  Terminate;
end;

constructor TQETScaler.Create(TheOwner: TComponent);
begin
  inherited Create(TheOwner);
  StopOnException:=True;
end;

destructor TQETScaler.Destroy;
begin
  inherited Destroy;
end;

procedure TQETScaler.WriteHelp;
var
  sExeName: string;
begin
  sExeName := ExtractFileName(ExeName);
  writeln();
  writeln(sExeName, ' version ', sVersion, ' needs some arguments!');
  writeln();
  writeln('usage for simple mode (both directions use the same factor):');
  writeln(sExeName, ' [-i|--stdin] [-o|--stdout] <file>  <scaling-factor>');
  writeln();
  writeln('With option "-o" or "--stdout" the scaled data is written to stdout. ');
  writeln('Otherwise the new data is written to a renamed file.');
  writeln();
  writeln('With option "-i" or "--stdin" input-data is read from stdin, a given');
  writeln('filename is ignored and the scaled element will be written to stdout.');
  writeln();
  writeln('In extended mode the scaling-factors for X and Y may differ and it is');
  writeln('allowed to specify only one direction for scaling. In each case the');
  writeln('Font-Sizes and Circle-Diameters are scaled by the smaller value.');
  writeln();
  writeln('usage for extended mode:');
  writeln(sExeName, ' [-i] [-o] [-x FactorForX] [-y FactorForY] -f FILENAME');
  writeln('or');
  writeln(sExeName, ' [-i] [-o] [-x FactorForX] [-y FactorForY] --file=FILENAME');
  writeln('');
  writeln('without parameters or with "-h" or "--help" this help is displayed');
  writeln('');
end;

var
  Application: TQETScaler;
begin
  Application:=TQETScaler.Create(nil);
  Application.Title:='QET_ElementScaler';
  Application.Run;
  Application.Free;
end.

