@echo scale QET - Element-Files ...

@echo off & setlocal

rem In der Variablen %cd% steht das aktuelle Verzeichnis!
rem --> kann genutzt werden, um die Batch in den "Pfad"
rem     zu legen und trotzdem im aufrufenden Verzeichnis
rem     die Dateien zu bearbeiten.

rem The variable %cd% contains the current directory!
rem --> can be used to place the batch in the "path" and
rem     still process the files in the calling directory..

rem Wo liegt das Programm?
rem Where does the Program lie?
set "ProgPfad=C:\Tools\QET_ElementScaler"

rem Wo liegen die Element-Dateien?
rem Where do the element-files reside?
set "DataPfad=Z:\win\Elemente"

rem Welcher Faktor soll angewendet werden?
rem Which factor do you want to apply?
set "Skalierung=0.9"


rem ------  Ab hier nichts mehr ändern!  ------
rem ------  No changes below this line!  ------


rem Recursively finds all matching files:
@for /r "%DataPfad%" %%i in ("*.elmt") do (
rem @for /r "%cd%" %%i in ("*.elmt") do (
    rem edit found files:
    "%ProgPfad%\QET_ElementScaler.exe" "%%i" %Skalierung%
    )

@echo + + + Fertig! + + +
