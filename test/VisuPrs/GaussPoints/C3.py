# This case corresponds to: /visu/GaussPoints/C3 case
# Create Gauss Points on the field of the MED file

import os
import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import GaussPointsOnField, EntityType, get_time, process_prs_for_test
import paravis
import pvsimple


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "GaussPoints/C3")
if not picturedir.endswith(os.sep):
    picturedir += os.sep

# MED file
file_name = datadir + "UO2_250ans_236.med"
field_name = "Concentration_O2(aq)_mol_per_l"
timestamp_nb = 1

paravis.myParavis.ImportFile(file_name)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    raise RuntimeError("File wasn't imported!!!")

# Create Gauss Points presentation
prs = GaussPointsOnField(med_reader, EntityType.CELL, field_name, timestamp_nb)
if prs is None:
    raise RuntimeError, "Created presentation is None!!!"

# Display presentation and get snapshot
view = pvsimple.GetRenderView()
time = get_time(med_reader, timestamp_nb)

pic_name = picturedir + field_name + "_" + str(time) + "_GAUSSPOINTS." + pictureext
process_prs_for_test(prs, view, pic_name)
