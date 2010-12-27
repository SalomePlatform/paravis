# This case corresponds to: /visu/SWIG_scripts/A3 case

from paravistest import datadir
from presentations import *
import paravis
import pvsimple


my_paravis = paravis.myParavis

print 'Importing "TimeStamps_236.med"...............',
file_path = datadir + "TimeStamps_236.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

print "Creating Cut Lines........................",
med_field = "vitesse"
cutlines = CutLinesOnField(med_reader, EntityType.NODE, med_field, 1,
                           nb_lines=20,
                           orientation1=Orientation.XY,
                           orientation2=Orientation.ZX)
if cutlines is None:
    print "FAILED"
else:
    print "OK"

print "Getting a viewer.........................",
view = pvsimple.GetRenderView()
if view is None:
    print "FAILED"
else:
    reset_view(view)
    print "OK"

cutlines.Visibility = 1
view.ResetCamera()
pvsimple.Render()

print "Creating an Animation.....................",
scalarmap = ScalarMapOnField(med_reader, EntityType.NODE, med_field, 2)

scene = pvsimple.AnimateReader(med_reader, view)
if scene is None:
    print "FAILED"
else:
    print "OK"
