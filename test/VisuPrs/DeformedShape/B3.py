# This case corresponds to: /visu/DeformedShape/B3 case

from paravistest import datadir
from presentations import DeformedShapeOnField, EntityType
import paravis
import pvsimple


my_paravis = paravis.myParavis

#====================Stage1: Import from MED file in ParaVis============
print "**** Stage1: Import from MED file in ParaVis"

print 'Import "Tria3.med"....................',

file_path = datadir + "Tria3.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

#====================Stage2: Creating Deformed Shape====================
print "**** Stage2: Creating Deformed Shape"

print "Creating Deformed Shape...............",

cell_entity = EntityType.CELL
field_name = 'vectoriel_field'
defshape = DeformedShapeOnField(med_reader, cell_entity, field_name, 1)
pvsimple.ResetCamera()

if defshape is None:
    print "FAILED"
else:
    print "OK"

#====================Stage3: Scale of Deformed Shape====================
print "**** Stage3: Scale of Deformed Shape"

warp_vector = pvsimple.GetActiveSource()
print "Default scale: ", warp_vector.ScaleFactor

print "Set positive scale of Deformed Shape"
scale = 1
warp_vector.ScaleFactor = scale

pvsimple.Render()
print "Scale: ", warp_vector.ScaleFactor

print "Set negative scale of Deformed Shape"
scale = -1
warp_vector.ScaleFactor = scale

pvsimple.Render()
print "Scale: ", warp_vector.ScaleFactor

print "Set zero scale of Deformed Shape"
scale = 0
warp_vector.ScaleFactor = scale

pvsimple.Render()
print "Scale: ", warp_vector.ScaleFactor

#====================Stage4: Coloring method of Deformed Shape===========
print "**** Stage4: Coloring of Deformed Shape"

color_array = defshape.ColorArrayName
if color_array:
    print "Default shape is colored by array: ", color_array
else:
    print "Default shape is colored by solid color: ", defshape.AmbientColor

print "Set colored by array mode    .... ",
defshape.ColorArrayName = field_name
pvsimple.Render()

if defshape.ColorArrayName == field_name:
    print "OK"
else:
    print "FAILED"

print "Set colored by solid color mode .... ",
defshape.ColorArrayName = ''
pvsimple.Render()

if defshape.ColorArrayName:
    print "FAILED"
else:
    print "OK"
