#This case corresponds to: /visu/ScalarMap_On_DeformedShape/F8 case
#%Create Scalar Map for all fields of the the given MED file%

from paravistest import *
from presentations import *
import paravis

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "maill.2_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.DEFORMEDSHAPESCALARMAP], picturedir, pictureext)


