# This case corresponds to: /visu/Plot3D/E2 case
# Create Plot 3D for all fields of the the given MED file

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "Plot3D/E2")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "cas2_2d_couplage_chess_castem_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.PLOT3D], picturedir, pictureext)