# This case corresponds to: /visu/CutPlanes/F9 case
# Create Cut Planes for all fields of the the given MED file

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "CutPlanes/F9")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "homard_ASTER_OSF_MEDV2.1.5_1_v2.2_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.CUTPLANES], picturedir, pictureext)


