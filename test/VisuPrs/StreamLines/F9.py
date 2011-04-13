# This case corresponds to: /visu/StreamLines/F9 case

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import StreamLinesOnField, EntityType, \
     reset_view, process_prs_for_test
import paravis
import pvsimple


my_paravis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "StreamLines/F9")

# Import of the "Bug829_resu_mode.med" file
file_path = datadir + "Bug829_resu_mode.med"

my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    raise RuntimeError("Bug829_resu_mode.med was not imported!!!")

# Get view
view = pvsimple.GetRenderView()

# Create a set of Stream Lines, based on time stamps of "MODES_DEPL" field
print "BREAKPOINT_1"

for i in range(1, 11):
    prs = StreamLinesOnField(med_reader, EntityType.NODE, "MODES_DEPL", i)
    if prs is None:
        raise RuntimeError("Presentation on timestamp {0} is None!!!".
                           format(i))

    picture_path = picturedir + "/" + "time_stamp_" + str(i) + "." + pictureext
    process_prs_for_test(prs, view, picture_path)

print "BREAKPOINT_2"
