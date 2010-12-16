# This case corresponds to: /visu/DeformedShape/E0 case
# Create Deformed Shape for all data of the given MED file

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

# Create presentations 
myParavis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1],"DeformedShape/E0") 

file = datadir + "test_55_solid_concentr_dom_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "CreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.DEFORMEDSHAPE], picturedir, pictureext)
