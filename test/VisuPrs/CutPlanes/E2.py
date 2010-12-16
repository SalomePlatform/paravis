# This case corresponds to: /visu/CutPlanes/E2 case
# Create Cut Planes for all data of the given MED file

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

# Create presentations 
myParavis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1],"CutPlanes/E2") 

file = datadir + "cas2_2d_couplage_chess_castem_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "CreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.CUTPLANES], picturedir, pictureext)
