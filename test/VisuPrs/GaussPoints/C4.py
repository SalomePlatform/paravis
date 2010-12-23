# This case corresponds to: /visu/GaussPoints/C4 case
# Create Gauss Points on the field of the MED file

import os
import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import GaussPointsOnField, EntityType, get_time, process_prs_for_test
import paravis
import pvsimple


# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "GaussPoints/C4")
if not picturedir.endswith(os.sep):
    picturedir += os.sep

# MED file
file_name = datadir + "homard_ASTER_OSF_MEDV2.1.5_1_v2.3.med"
field_names = ["REMEUN_ERRE_ELGA_NORE", "REMEZEROERRE_ELGA_NORE"]
timestamp_nb = 1

paravis.myParavis.ImportFile(file_name)
med_reader = pvsimple.GetActiveSource()
if med_reader is None:
    raise RuntimeError("File wasn't imported!!!")

# Create Gauss Points presentation
view = pvsimple.GetRenderView()
time = get_time(med_reader, timestamp_nb)

for field_name in field_names:
    prs = GaussPointsOnField(med_reader, EntityType.CELL, field_name, timestamp_nb)
    if prs is None:
        raise RuntimeError, "Created presentation is None!!!"

    # Display presentation and get snapshot
    pic_name = "{folder}{field}_{time}_{type}.{ext}".format(folder=picturedir,
                                                            field=field_name,
                                                            time=time,
                                                            type="GAUSSPOINTS",
                                                            ext=pictureext)
    process_prs_for_test(prs, view, pic_name)
