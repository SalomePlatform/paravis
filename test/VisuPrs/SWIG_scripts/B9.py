# This case corresponds to: /visu/SWIG_scripts/B9 case

from paravistest import datadir
from presentations import EntityType, ScalarMapOnField, CutPlanesOnField
import paravis
import pvsimple


my_paravis = paravis.myParavis

# Get view
view1 = pvsimple.GetRenderView()

# Import MED file
med_file = datadir + "pointe.med"
field_name = "fieldnodedouble"
entity = EntityType.NODE
timestamp = 1

my_paravis.ImportFile(med_file)
med_reader = pvsimple.GetActiveSource()

# Create presentations
scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp)
pvsimple.ResetCamera(view1)

view2 = pvsimple.CreateRenderView()
cutlines = CutPlanesOnField(med_reader, entity, field_name, timestamp)
pvsimple.ResetCamera(view2)

# Delete
source = cutlines.Input
pvsimple.Delete(source)
pvsimple.Delete(med_reader)

# Clear views from scalar bar and update views
for rview in pvsimple.GetRenderViews():
    rview.Representations.Clear()
    pvsimple.Render(rview)
