#This case corresponds to: /visu/ScalarMap_On_DeformedShape/F1 case
#%Create Scalar Map on Deformed Shape for all fields of the the given MED file%

from paravistest import * 
from presentations import *
import paravis

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "ml_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.DEFORMEDSHAPESCALARMAP], picturedir, pictureext)

