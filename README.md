# QET_ElementScaler

QET_ElementScaler is a commandline-tool to scale QElectroTech-Elements with constant factor(s) and save the changed data to a renamed file. With the optional parameter &quot;-s&quot; the changed data is written to stdout and no new file is created.

It compiles with Lazarus 2.2.2 and FreePascal 3.2.2 on Debian/GNU Linux (unstable) and ReactOS (0.4.15-dev-4888).
The ReactOS-Build can also be used with Win in a cmd-window.<br>

QET_ElementScaler can process one file at a time.
If you want to edit all files in a directory, you need to call QET_ElementScaler from a batch-, cmd- or shell-script-file such as the ones available here.<br>

You do not want to (or can't) compile the FreePascal program yourself? Download the executable file suitable for your operating system from the releases-page and use that.<br><br>
Hint: <br>
Get familiar with the command line before using this software.
General support for using the command line for the various systems cannot be provided here.



usage:<br>
QET_ElementScaler  [-s] &lt;file&gt;  &lt;scaling-factor&gt; <br>
(as used in Batch-File &quot;scale.cmd&quot; and shell-script &quot;scale.sh&quot;)

or:<br>
QET_ElementScaler [-s] [-x FactorForX] [-y FactorForY] -f FILENAME


