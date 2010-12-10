#In base before redesign (tag: V3_2_0b1, for ex.) such case corresponds to: /visu/007/A2 case
# VISU_Gen interface methods: SetCurrentStudy,DeformedShapeOnField
# DeformedShape interface methods: SetScale,GetScale,IsColored,ShowColored
# Created by enk

import SALOMEDS
import os

#%====================Stage1: Creating a new study====================%
print "**** Stage1: Creating a new study "

print "Creating a new study.................."
myVisu=myVisu
myVisu.SetCurrentStudy(salome.myStudy)

#%====================Stage2: Import from MED file in Visu====================%
print "**** Stage2: Import from MED file in Visu"

print 'Import "Tria3_236.med"....................',
medHexa8=datadir + "Tria3_236.med"
myResult=myVisu.ImportFile(medHexa8)
if myResult is None : print "Error"
else : print "OK"

#%====================Stage3: Creating Deformed Shape====================%
print "**** Stage3: Creating Deformed Shape"

print "Creating Deformed Shape...............",
myMeshName           = "Maillage MED_TRIA3"
myCellEntity         = VISU.CELL
myVectorielFieldName = 'vectoriel field'
defshape=myVisu.DeformedShapeOnField(myResult,myMeshName,myCellEntity,myVectorielFieldName,1);
if defshape is None : print "Error"
else : print "OK"

#%====================Stage4: Scale of Deformed Shape====================%
print "**** Stage4: Scale of Deformed Shape"

print "Default scale : ",defshape.GetScale()

print "Set positive scale of Deformed Shape"
aScale=1
defshape.SetScale(aScale)
print "Scale : ",defshape.GetScale()

print "Set negative scale of Deformed Shape"
aScale=-1
defshape.SetScale(aScale)
print "Scale : ",defshape.GetScale()

print "Set zero scale of Deformed Shape"
aScale=0
defshape.SetScale(aScale)
print "Scale : ",defshape.GetScale()

#%====================Stage5: IsColored method of Deformed Shape====================%
print "**** Stage5: IsColored of Deformed Shape"

if defshape.IsColored():
    print "Default shape is Colored"
else:
    print "Default shape is not Colored"

print "Set colored mode    .... ",
defshape.ShowColored(1)
if defshape.IsColored():
    print "OK"
else:
    print "Error"

print "Set not colored mode .... ",
defshape.ShowColored(0)
if defshape.IsColored():
    print "Error"
else:
    print "OK"
        
