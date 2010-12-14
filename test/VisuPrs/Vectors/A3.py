# This case corresponds to: /visu/Vectors/A1 case
# Create Vectors for all fields of the the given MED file

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "Vectors/A3")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "Fields_group3D_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.VECTORS], picturedir, pictureext)
