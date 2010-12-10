#This case corresponds to: /visu/Plot3D/F8 case
#%Create Plot 3D for all fields of the the given MED file%

import sys
from paravistest import * 
from presentations import *
import paravis

picturedir = get_picture_dir(sys.argv[1], "Plot3D/F8")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "homard_ASTER_OSF_MEDV2.1.5_1_v2.1_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.PLOT3D], picturedir, pictureext)
