#!/bin/sh

# scales QET - Element-files in the directory and all sub-dirs
# by the factor given here:

Skalierung="0.5"
SkalierProg="/usr/local/bin/QET_ElementScaler"

# um Leerzeichen in Dateinamen verarbeiten zu können:
# Originalzustand merken:
OFS=$IFS
# Einstellen:
IFS="
"

for i in  `find . -name "*.elmt"` ; do
  echo "processing $i"
  # Grafik skalieren:
  "$SkalierProg" "$i" "$Skalierung"
  done

for i in  `find . -name "*.SCALED.elmt"` ; do
  echo "processing $i"
  # "xml version" entfernen
  grep -v -i "xml version" "$i" > /tmp/tempfile.elmt
  # Dateiname wieder herstellen
  cat /tmp/tempfile.elmt > "$i"
  done

# aufräumen:
rm -r /tmp/tempfile.elmt

# Leerzeichen in Dateinamen - wieder Originalzustand:
IFS=$OFS
# FERTIG!
