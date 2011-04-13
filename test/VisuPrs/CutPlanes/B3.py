# This case corresponds to: /visu/CutPlanes/B3 case

from paravistest import datadir
from presentations import CutPlanesOnField, EntityType
import paravis
import pvsimple

my_paravis = paravis.myParavis

#====================Stage1: Importing MED file====================
print "**** Stage1: Importing MED file"

print 'Import "ResOK_0000.med"...............',

file_path = datadir + "ResOK_0000.med"
my_paravis.ImportFile(file_path)
med_reader = pvsimple.GetActiveSource()

if med_reader is None:
    print "FAILED"
else:
    print "OK"

#====================Stage2: Creating CutPlanes====================
print "*****Stage2: Creating CutPlanes"

print "Creating Cut Planes.......",

node_entity = EntityType.NODE
field_name = 'vitesse'
cutplanes = CutPlanesOnField(med_reader, node_entity, field_name, 1)

if cutplanes is None:
    print "FAILED"
else:
    print "OK"
