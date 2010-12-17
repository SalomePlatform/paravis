#This case corresponds to: /visu/animation/A0 case
#%Create CutLines for 'vitesse' field of the the given MED file and creates animation on this field %

import sys
from paravistest import * 
from presentations import *
from pvsimple import *
import paravis

# Create presentations
myParavis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "Animation/A0")

theFileName = datadir +  "TimeStamps_236.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "

myParavis.ImportFile(theFileName)
aProxy = GetActiveSource()
if aProxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"

print "Creating Cut Lines........................",
cut_lines = CutLinesOnField(aProxy,EntityType.NODE,'vitesse' , 1,\
theNbLines=20,theOrientation1 = Orientation.XY, theOrientation2 = Orientation.ZX)

if cut_lines is None : print "Error"
else : print "OK"

print "Creating a Viewer.........................",
aView = GetRenderView()
reset_view(aView)
Render(aView)

if aView is None : print "Error"
else : print "OK"

cut_lines.Visibility=1

prs= ScalarMapOnField(aProxy,EntityType.NODE,'vitesse' , 1)
prs.Opacity=0.5
print "Creating an Animation.....................",
scene = AnimateReader(aProxy,aView)
scene.PlayMode = 1
scene.FramesPerTimestep = 4
scene.Loop = 1
print "Animation.................................",
scene.Play()

