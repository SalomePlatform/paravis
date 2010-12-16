# This case corresponds to: /visu/MeshPresentation/E8 case
# Create Mesh Presentation for all data of the given MED file

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

# Create presentations 
myParavis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1],"MeshPresentation/E8") 

file = datadir + "KCOUPLEX1_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "CreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.MESH], picturedir, pictureext)
