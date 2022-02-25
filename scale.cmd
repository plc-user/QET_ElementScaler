@echo skaliere QET - Element-Dateien ...

@echo off & setlocal

rem Wo liegt das Programm?
rem Where does the Program lie?
set "ProgPfad=C:\Tools\QET_ElementScaler"

rem Wo liegen die zu skalierenden Element-Dateien?
rem Path to the directory of element-files to be scaled:
set "DataPfad=Z:\win\Elemente"

rem Welcher Faktor soll angewendet werden?
set "Skalierung=0.9"


rem ------  Ab hier nichts mehr ändern!  ------
rem ------  No changes below this line!  ------


rem Findet rekursiv alle passenden Dateien:
@for /r "%DataPfad%" %%i in ("*.elmt") do (
    rem gefundene Dateien bearbeiten:
    "%ProgPfad%\QET_ElementScaler.exe" "%%i" %Skalierung%
    )


rem Rekursiv den XML-Header entfernen:
@for /r "%DataPfad%" %%i in ("*.SCALED.elmt") do (
    rem gefundene Dateien bearbeiten:
    more +1 < "%%i" > "%%i.txt"
    rem und wieder umbenennen:
    move /Y "%%i.txt" "%%i"
    )


@echo + + + Fertig! + + +
