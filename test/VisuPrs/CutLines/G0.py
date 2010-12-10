#This case corresponds to: /visu/CutLines/G0 case
#%Create Cut Lines for all fields of the the given MED file%

import sys
from paravistest import * 
from presentations import *
import paravis


# Create presentations
myParavis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "CutLines/G0")

file = datadir +  "homard_ASTER_OSF_MEDV2.1.5_1_v2.3_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.CUTLINES], picturedir, pictureext)


