#This case corresponds to: /visu/CutPlanes/F0 case
#%Create Cut Planes for all fields of the the given MED file%

import sys
from paravistest import * 
from presentations import *
import paravis

# Create presentations
myParavis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "CutPlanes/F0")

file = datadir +  "gro5couches_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.CUTPLANES], picturedir, pictureext)


