# This case corresponds to: /visu/IsoSurfaces/A1 case
# Create Iso Surface for all data of the given MED file

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

# Create presentations 
myParavis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1],"IsoSurfaces/A1") 

file = datadir + "hexa_28320_ELEM.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "CreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.ISOSURFACES], picturedir, pictureext)
