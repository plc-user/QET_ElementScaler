# QET_ElementScaler

QET_ElementScaler is a commandline-tool to scale QElectroTech-Elements with constant factor(s) and save the changed data to a renamed file.
With the optional parameter &quot;-o&quot; or &quot;--stdout&quot; the changed data is written to stdout and no new file is created.


It compiles with C++17 activated on Debian/GNU Linux (unstable), ReactOS (0.4.15-dev-5359) and macOS (darwin 21.6.0.0).
The ReactOS-Build can also be used with Win in a cmd-window.<br>

QET_ElementScaler can process one file at a time.
If you want to edit all files in a directory, you need to call QET_ElementScaler from a batch-, cmd- or shell-script-file such as the ones available here.<br>

You do not want to (or can't) compile the program yourself? Download the executable file suitable for your operating system from the releases-page and use that.<br><br>
Hint: <br>
Get familiar with the command line before using this software.
General support for using the command line for the various systems cannot be provided here.



usage:<br>
QET_ElementScaler [options] &lt;file&gt;  &lt;scaling-factor&gt; <br>
(as used in Batch-File &quot;scale.cmd&quot; and shell-script &quot;scale.sh&quot;) <br>

or:<br>
QET_ElementScaler [options] [-x FactorForX] [-y FactorForY] -f FILENAME <br>

 <br>

  available options:
```
   -i | --stdin     input-data is read from stdin, a given filename is
                    ignored and scaled element will be written to stdout
   -o | --stdout    output will be written to stdout
   -x VALUE         or 
   --factorx VALUE  factor for x-values (x, rx, width, ...) 
   -y VALUE         or 
   --factory VALUE  factor for y-values (y, ry, height, ...) 
   -f FILENAME      or 
   --file FILENAME  the file that will be used 
   -h | --help      show this help 
```

  there are also some "long-opt"-only options: 
```
  "--RemoveAllTerminals" removes all terminals from the element 
                         (useful for front-views or "thumbnails") 
  "--FlipPolyHorizontal" flips ALL Lines and polygons horizontally 
                         (useful during creation of elements) 
  "--FlipPolyVertical"   flips ALL Lines and polygons vertically 
                         (useful during creation of elements) 
  "--OverwriteOriginal"  the original file is replaced by scaled one 
                         (CAUTION: Be careful with this option!) 
```


 <br>
examples to use data from stdin: <br>
QET_ElementScaler -i  2.0  &lt;  ElementToScale.elmt  &gt;  ScaledElement.elmt <br>
 <br>
or use a pipe like this <br>
cat  ElementToScale.elmt  |  QET_ElementScaler -i  2.0  |  OtherSoftwareBinary <br>
 <br>
extended mode is also possible:<br>
QET_ElementScaler -i -x 2 -y 3  &lt;  ElementToScale.elmt  &gt;  ScaledElement.elmt <br>
 <br>
 <br>

QET_ElementScaler uses Arseny Kapoulkine's &quot;pugixml&quot; (https://github.com/zeux/pugixml)
to handle the content of the Element-File. Thank you, Arseny! <br>
 <br>
