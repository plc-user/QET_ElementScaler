# QET_ElementScaler

QET_ElementScaler is a commandline-tool to scale QElectroTech-Graphics with constant factor(s).

It compiles with Lazarus 2.0.12 and FreePascal 3.2.0 on Debian/GNU Linux (unstable) and ReactOS (0.4.15-dev-3081).
The ReactOS-Build can also be used with Win in a cmd-window.<br>

QET_ElementScaler can process one file at a time.
If you want to edit all files in a directory, you need to call QET_ElementScaler from a batch-, cmd- or shell-script-file such as the ones available here.<br>

You do not want to (or can't) compile the FreePascal program yourself? Download the executable file suitable for your operating system from the releases-page and use that.<br><br>
Hint: <br>
Get familiar with the command line before using this software.
General support for using the command line for the various systems cannot be provided here.



usage:<br>
QET_ElementScaler  &lt;file&gt;  &lt;scaling-factor&gt; <br>
(as used in Batch-File &quot;scale.cmd&quot; and shell-script &quot;scale.sh&quot;)

or:<br>
QET_ElementScaler [-x FactorForX] [-y FactorForY] -f FILENAME


