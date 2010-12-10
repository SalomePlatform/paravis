#This case corresponds to: /visu/Vectors/B1 case
#%Create Vectors for all fields of the the given MED file%

from paravistest import * 
from presentations import *
import paravis

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "carre_en_quad4_seg2_fields_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.VECTORS], picturedir, pictureext)
