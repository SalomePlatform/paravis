#This case corresponds to: /visu/ScalarMap/E9 case
#%Create Scalar Map for all fields of the the given MED file%

from paravistest import *
from presentations import *
import paravis

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "UO2_250ans_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.SCALARMAP], picturedir, pictureext)


