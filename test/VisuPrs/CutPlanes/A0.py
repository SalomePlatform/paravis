#This case corresponds to: /visu/CutPlanes/A0 case
#%Create Cut Planes for all fields of the the given MED file%

from paravistest import * 
from presentations import *
import paravis

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "fra.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.CUTPLANES], picturedir, pictureext)


