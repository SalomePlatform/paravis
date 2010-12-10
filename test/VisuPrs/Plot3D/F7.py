#This case corresponds to: /visu/Plot3D/F7 case
#%Create Plot 3D for all fields of the the given MED file%

import sys
from paravistest import * 
from presentations import *
import paravis


picturedir = get_picture_dir(sys.argv[1], "Plot3D/F7")
# Create presentations
myParavis = paravis.myParavis

file = datadir +  "occ4050.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.PLOT3D], picturedir, pictureext)
