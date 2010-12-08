#This case corresponds to: /visu/MeshPresentation/A1 case
#Create all possible submeshes on the given MED file

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

# Create presentations
myParavis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "MeshPresentation/A1")

file = datadir +  "hexa_28320_ELEM_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.MESH], picturedir, pictureext)

