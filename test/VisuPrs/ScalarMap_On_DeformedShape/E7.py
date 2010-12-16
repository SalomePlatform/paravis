# This case corresponds to: /visu/ScalarMap_On_DeformedShape/E7 case
# Create Scalar Map on Deformed Shape for all data of the given MED file

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

# Create presentations 
myParavis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1],"ScalarMap_On_DeformedShape/E7") 

file = datadir + "couplex_alain_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "CreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.DEFORMEDSHAPESCALARMAP], picturedir, pictureext)
