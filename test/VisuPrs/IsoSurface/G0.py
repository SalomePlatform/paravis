#This case corresponds to: /visu/IsoSurface/G0 case
#%Create Iso Surface for all fields of the the given MED file%

import sys
from paravistest import * 
from presentations import *
import paravis


picturedir = get_picture_dir(sys.argv[1], "IsoSurface/G0")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "homard_ASTER_OSF_MEDV2.1.5_1_v2.3_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.ISOSURFACES], picturedir, pictureext)
