program QET_ElementScaler;

//
// "Quick-And-Dirty"-approach to scale QElectroTech-Graphics with a constant factor.
//
// compiles/runs with Lazarus 2.0.10 and FreePascal 3.2.0 on
// Debian/GNU Linux (unstable) and ReactOS (0.4.15-dev-1196)
//
// usage:
// QET_ElementScaler <file> <scaling-factor>
//
// Result is a XML-File with an additional header-line
// <?xml version="1.0" encoding="utf-8"?>
// but it does not seem to bother QET   ;-)
//
// use it at your own risk!
//
// Date: 17.12.2020
// Author: plc-user
// https//github.com/plc-user
//
// based on example-code from
// https://wiki.freepascal.org/XML_Tutorial
//


uses
  Classes, SysUtils,
  DOM, XMLRead, XMLWrite;

var
  QETfile: TXMLDocument;   // holds the XML-Data from/to file
  gfFactor: double = 1.0;  // Scaling-Factor for graphics
  {$IFDEF WINDOWS}
  cDecSep: char;           // Decimal-Separator
  {$Endif}



function FaktorAnwenden(elem, name, val: string): string;
// multiplies the Value "s" with factor and
// returns a value with max. 2 decimals
// removes trailing zeros
var
  fNewVal: double = 0.0;
  slFont: TStringList;
begin
  if ((elem = 'text') or (elem = 'dynamic_text')) and (name = 'font') then begin
    slFont := TStringList.Create;
    slFont.AddDelimitedText(val,',',true);
    slFont[1] := (Format('%0.2f', [(slFont[1].ToDouble * gfFactor)]));
    if (slFont[1][length(slFont[1])] = '0') then  slFont[1] := Copy(slFont[1], 1 ,length(slFont[1])-1);
    if (slFont[1][length(slFont[1])] = '0') then  slFont[1] := Copy(slFont[1], 1 ,length(slFont[1])-1);
    if (slFont[1][length(slFont[1])] = '.') then  slFont[1] := Copy(slFont[1], 1 ,length(slFont[1])-1);
    val := slFont.CommaText;
    val := StringReplace(val, '"' , '',[rfReplaceAll]);
    end;
  if ((elem = 'polygon') and ((name[1] = 'x') or (name[1] = 'y'))) or
     ((elem = 'definition') and ((name = 'height') or (name = 'width') or (name = 'hotspot_x') or (name = 'hotspot_y'))) or
     ((elem = 'rect') and ((name = 'height') or (name = 'width') or (name = 'x') or (name = 'y') or (name = 'rx') or (name = 'ry'))) or
     ((elem = 'line') and ((name = 'x1') or (name = 'x2') or (name = 'y1') or (name = 'y2'))) or
     ((elem = 'dynamic_text') and ((name = 'x') or (name = 'y'))) or
     ((elem = 'text') and ((name = 'x') or (name = 'y'))) or
     ((elem = 'terminal') and ((name = 'x') or (name = 'y'))) or
     ((elem = 'arc') and ((name = 'height') or (name = 'width') or (name = 'x') or (name = 'y'))) or
     ((elem = 'circle') and ((name = 'x') or (name = 'y') or (name = 'diameter'))) or
     ((elem = 'ellipse') and ((name = 'x') or (name = 'y') or (name = 'height') or (name = 'width')))
    then begin
      fNewVal := (strtofloat(val) * gfFactor);
      result := (Format('%0.2f', [fNewVal]));
      if (elem = 'definition') then begin
        result := inttostr(round(fNewVal));
        exit;
        end;
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '0') then  result := Copy(result, 1 ,length(result)-1);
      if (result[length(result)] = '.') then  result := Copy(result, 1 ,length(result)-1);
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
        //writeln(Node.NodeName);
        // hier die Attribute durchgehen und alle Maß-Angaben ändern
        for i := 0 to (Node.Attributes.Length-1) do begin
          //writeln('ALT: '+Node.Attributes[i].NodeName +' = '+Node.Attributes[i].NodeValue);
          Node.Attributes[i].NodeValue := FaktorAnwenden(Node.NodeName,
                                                         Node.Attributes[i].NodeName,
                                                         Node.Attributes[i].NodeValue);
          //writeln('NEU: '+Node.Attributes[i].NodeName +' = '+Node.Attributes[i].NodeValue);
          end;
        //writeln();
      end;
    // Zum Kindknoten weiter gehen
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
    //writeln(iNode.ParentNode.NodeName);
    for i := 0 to (iNode.ParentNode.Attributes.Length-1) do begin
      //writeln('ALT: '+iNode.ParentNode.Attributes[i].NodeName +' = '+iNode.ParentNode.Attributes[i].NodeValue);
      iNode.ParentNode.Attributes[i].NodeValue := FaktorAnwenden(iNode.ParentNode.NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeName,
                                                               iNode.ParentNode.Attributes[i].NodeValue);
      //writeln('NEU: '+iNode.ParentNode.Attributes[i].NodeName +' = '+iNode.ParentNode.Attributes[i].NodeValue);
      end;
    end;
  while iNode <> nil do
  begin
    ProcessNode(iNode); // Rekursiv
    iNode := iNode.NextSibling;
  end;
end;





procedure PrintUsage;
begin
  writeln(ParamStr(0), ' takes 2 arguments!');
  writeln('usage:');
  writeln(ParamStr(0), ' <file> <scaling-factor>');
  writeln();
end;





begin
  // Remember Setting for Decimal-Separator:
  {$IFDEF WINDOWS}
  cDecSep := DefaultFormatSettings.DecimalSeparator;
  DefaultFormatSettings.DecimalSeparator := '.';
  {$Endif}

  // Check Commandline-Parameters
  if ParamCount <> 2 then begin
    PrintUsage();
    exit;
  end;
  if not(FileExists(Paramstr(1))) then begin
    writeln('File not found: ',ParamStr(1));
    exit;
  end;
  try
    gfFactor := ParamStr(2).ToDouble;
  except
    writeln('Not a valid number: ',ParamStr(2));
    exit;
  end;

// Parameters seem to be o.k. --> carry on!

  // Read data to structure:
  ReadXMLFile(QETfile, ParamStr(1));

  // process data-conversion:
  ScaleGraphics();

  // save changed data to new file
  WriteXMLFile(QETfile, changeFileExt(Paramstr(1), '.SCALED.elmt'));

  {$IFDEF WINDOWS}
  DefaultFormatSettings.DecimalSeparator := cDecSep;
  {$Endif}
end.

