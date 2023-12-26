#!/bin/sh

# scales QET - Element-files in the directory from where it was called and
# all sub-dirs by the factor given here:

Skalierung="5.0"
SkalierProg="/home/ich/Projekte/c_c++/QET_ElementScaler/QET_ElementScaler"

# to be able to process blanks in file names - remember original state:
OFS=$IFS
# set new:
IFS="
"

for i in  `find . -name "*.elmt"` ; do
  echo "processing $i"
  # With the construct in curly brackets the file extension
  # is changed accordingly
  "$SkalierProg" --toSVG -o -F "$Skalierung""$i" > "${i%.elmt}.svg"
  done

# blanks in filenames - back to original:
IFS=$OFS
# DONE!
