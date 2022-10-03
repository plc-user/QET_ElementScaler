# QET_ElementScaler

QET_ElementScaler is a commandline-tool to scale QElectroTech-Elements with constant factor(s) and save the changed data to a renamed file.
With the optional parameter &quot;-o&quot; or &quot;--stdout&quot; the scaled element is written to stdout and no new file is created.
The optional parameter &quot;-i&quot; or &quot;--stdin&quot; is used to read the input-data from stdin and the output is forced to stdout.<br>

It compiles with Lazarus 2.2.2 and FreePascal 3.2.2 on Debian/GNU Linux (unstable) and ReactOS (0.4.15-dev-4888).
The ReactOS-Build can also be used with Win in a cmd-window.<br>

QET_ElementScaler can process one file at a time.
If you want to edit all files in a directory, you need to call QET_ElementScaler from a batch-, cmd- or shell-script-file such as the ones available here.<br>

You do not want to (or can't) compile the FreePascal program yourself? Download the executable file suitable for your operating system from the releases-page and use that.<br><br>
Hint: <br>
Get familiar with the command line before using this software.
General support for using the command line for the various systems cannot be provided here.<br>


usage:<br>
QET_ElementScaler  [-i] [-o] &lt;file&gt;  &lt;scaling-factor&gt; <br>
(as used in Batch-File &quot;scale.cmd&quot; and shell-script &quot;scale.sh&quot;)<br>

or:<br>
QET_ElementScaler [-i] [-o] [-x FactorForX] [-y FactorForY] -f FILENAME<br>

 <br>
examples to use data from stdin: <br>
QET_ElementScaler -i  2.0  &lt;  ElementToScale.elmt  &gt;  ScaledElement.elmt <br>
 <br>
or use a pipe like this <br>
cat  ElementToScale.elmt  |  QET_ElementScaler -i  2.0  |  OtherSoftwareBinary <br>
 <br>
extended mode and long-options are also possible:<br>
QET_ElementScaler --stdin -x 2 -y 3  &lt;  ElementToScale.elmt  &gt;  ScaledElement.elmt <br>
 <br>
