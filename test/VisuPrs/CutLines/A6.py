# This case corresponds to: /visu/CutLines/A6 case
# Create Cut Lines for all data of the given MED file

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

# Create presentations 
myParavis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1],"CutLines/A6") 

file = datadir + "clo.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "CreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.CUTLINES], picturedir, pictureext)
