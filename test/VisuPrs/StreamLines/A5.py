# This case corresponds to: /visu/StreamLines/A5 case
# Create Stream Lines for all fields of the the given MED file

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "StreamLines/A5")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "Penta6_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.STREAMLINES], picturedir, pictureext)
