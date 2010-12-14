# This case corresponds to: /visu/CutPlanes/B0 case
# Create Cut Planes for all fields of the the given MED file

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "CutPlanes/B0")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "carre_en_quad4_seg2_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.CUTPLANES], picturedir, pictureext)


