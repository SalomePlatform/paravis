#This case corresponds to: /visu/animation/A7 case
#%Create animation for 'pression' field of the the given MED file and dumps picture files in PNG format %

import sys
import os
from paravistest import * 
from presentations import *
from pvsimple import *
import paravis

# Create presentations
myParavis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "Animation/A7")

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
prs_0 = CutLinesOnField(aProxy,EntityType.CELL,'pression',1)

if prs_0 is None : print "Error"
else : print "OK"

print "Creating a Viewer.........................",
aView = GetRenderView()
reset_view(aView)
Render(aView)

if aView is None : print "Error"
else : print "OK"

prs_0.Visibility=1

prs= ScalarMapOnField(aProxy,EntityType.CELL,'pression' , 2)
prs.Opacity=0.5

print "Creating an Animation.....................",
my_format = "png"
print "Current format to save snapshots: ",my_format
# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

scene = AnimateReader(aProxy,aView,picturedir+"A7_dom."+my_format)
nb_frames = scene.NumberOfFrames

pics = os.listdir(picturedir) 
if len(pics) != nb_frames:
    print "FAILED!!! Number of made pictures is equal to ", len(pics), " instead of ", nb_frames
    
for pic in pics:
    os.remove(picturedir+pic)    
    
scene.FramesPerTimestep = 8
print "Animation.................................",
scene.Play()
scene.GoToFirst()
