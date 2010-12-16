# This case corresponds to: /visu/StreamLines/A6 case
# Create Stream Lines for all fields of the the given MED file

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "StreamLines/A6")

# Create presentations
myParavis = paravis.myParavis

file = datadir +  "clo_236.med"
print " --------------------------------- "
print "file ", file
print " --------------------------------- "
print "\nCreatePrsForFile..."
CreatePrsForFile(myParavis, file, [PrsTypeEnum.STREAMLINES], picturedir, pictureext)
