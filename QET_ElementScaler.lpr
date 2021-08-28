program QET_ElementScaler;

//
// Scale QElectroTech-Graphics with a constant factor(s).
//
// compiles/runs with Lazarus 2.0.12 and FreePascal 3.2.0 on
// Debian/GNU Linux (unstable) and ReactOS (0.4.15-dev-3081)
//
// usage:
// QET_ElementScaler <file> <scaling-factor>
//
// OR
// QET_ElementScaler [-x FactorForX] [-y FactorForY] -f FILENAME
//
//
// Result is a XML-File with an additional header-line
// <?xml version="1.0" encoding="utf-8"?>
// but it does not seem to bother QET   ;-)
//
// use it at your own risk!
//
// Last Change: 28.08.2021
// all UUIDs are replaced by new ones
//
// Created: 17.12.2020
// Author: plc-user
// https://github.com/plc-user
//
// based on example-code from
// https://wiki.freepascal.org/XML_Tutorial
//


{$mode objfpc}{$H+}

uses
  {$IFDEF UNIX}{$IFDEF UseCThreads}
  cthreads,
  {$ENDIF}{$ENDIF}
  Classes, SysUtils, CustApp,
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
  sVersion: string = '0.2beta4';

var
  QETfile: TXMLDocument;    // holds the XML-Data from/to file
  gfFactor:  double = 1.0;  // Scaling-Factor for circles and Text-size
  gfFactorX: double = 1.0;  // Scaling-Factor X-direction
  gfFactorY: double = 1.0;  // Scaling-Factor Y-direction
  cDecSep: char;            // Decimal-Separator

// Funktionen außerhalb der Class .............

function FaktorAnwenden(elem, name, val: string): string;
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
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '.') then  result := Copy(result, 1 ,length(result)-1);
    end
  else if ((name = 'uuid')) then begin
    if (CreateGUID(uuidNEW) = 0) then
      result := LowerCase(GUIDToString(uuidNEW));
    end
  else result := val;
end;


function FaktorXAnwenden(elem, name, val: string): string;
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
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '.') then  result := Copy(result, 1 ,length(result)-1);
    end
  else if ((name = 'uuid')) then begin
    if (CreateGUID(uuidNEW) = 0) then
      result := LowerCase(GUIDToString(uuidNEW));
    end
  else result := val;
end;


function FaktorYAnwenden(elem, name, val: string): string;
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
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '.') then  result := Copy(result, 1 ,length(result)-1);
    end
  else if ((name = 'uuid')) then begin
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
          Node.Attributes[i].NodeValue := FaktorAnwenden(Node.NodeName,
                                                         Node.Attributes[i].NodeName,
                                                         Node.Attributes[i].NodeValue);
          Node.Attributes[i].NodeValue := FaktorXAnwenden(Node.NodeName,
                                                         Node.Attributes[i].NodeName,
                                                         Node.Attributes[i].NodeValue);
          Node.Attributes[i].NodeValue := FaktorYAnwenden(Node.NodeName,
                                                         Node.Attributes[i].NodeName,
                                                         Node.Attributes[i].NodeValue);
          end;
      end;
    // zum Kindknoten weitergehen
    cNode := Node.ChildNodes.Item[0];
    // Alle Kindknoten bearbeiten
    while cNode <> nil do
    begin
      ProcessNoDe(cNode);
      cNode := cNode.NextSibling;
    end;
  end;

begin
  iNode := QETfile.DocumentElement.ChildNodes.Item[0];
  if iNode.ParentNode.HasAttributes then begin
    for i := 0 to (iNode.ParentNode.Attributes.Length-1) do begin
      iNode.ParentNode.Attributes[i].NodeValue := FaktorAnwenden(iNode.ParentNode.NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeValue);
      iNode.ParentNode.Attributes[i].NodeValue := FaktorXAnwenden(iNode.ParentNode.NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeValue);
      iNode.ParentNode.Attributes[i].NodeValue := FaktorYAnwenden(iNode.ParentNode.NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeValue);
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
begin
  // Remember Setting for Decimal-Separator:
  cDecSep := DefaultFormatSettings.DecimalSeparator;
  DefaultFormatSettings.DecimalSeparator := '.';

  // Schnelltest der Parameter
  ErrorMsg:=CheckOptions('hx:y:f:v', 'help file: version');
  if ErrorMsg<>'' then begin
    ShowException(Exception.Create(ErrorMsg));
    writeln(' * * * STOP * * *');
    Terminate;
    Exit;
  end;

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
  if (ParamCount < 2) then begin
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
  if (ParamCount = 2)
    then begin
      sDateiName := ParamStr(1);
      try
        gfFactor := ParamStr(2).ToDouble;
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

// bevor es losgeht, den Dateinamen prüfen:
  if (ExtractFileExt(sDateiName.ToLower) <> '.elmt') then begin
      writeln('invalid file-type: ', sDateiName);
      writeln(' * * * STOP * * *');
      Terminate;
      exit;
    end;
  if not(FileExists(sDateiName)) then begin
    writeln('File not found: ', sDateiName);
    writeln(' * * * STOP * * *');
    Terminate;
    exit;
  end;


  // Read data to structure:
  ReadXMLFile(QETfile, sDateiName);

  // process data-conversion:
  if (gfFactorX < gfFactorY)
    then gfFactor := gfFactorX
    else gfFactor := gfFactorY;
  ScaleGraphics();

  // save changed data to new file
  WriteXMLFile(QETfile, changeFileExt(sDateiName, '.SCALED.elmt'));

  // Einstellungen wieder auf Original:
  DefaultFormatSettings.DecimalSeparator := cDecSep;

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
  writeln(sExeName, '  <file>  <scaling-factor>');
  writeln();
  writeln('In extended mode the scaling-factors for X and Y may differ and it is');
  writeln('allowed to specify only one direction for scaling. In each case the');
  writeln('Font-Sizes and Circle-Diameters are scaled by the smaller value.');
  writeln('');
  writeln('usage for extended mode:');
  writeln(sExeName, ' [-x FactorForX] [-y FactorForY] -f FILENAME');
  writeln('or');
  writeln(sExeName, ' [-x FactorForX] [-y FactorForY] --file=FILENAME');
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

