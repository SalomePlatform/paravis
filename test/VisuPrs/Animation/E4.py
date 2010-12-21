# This case corresponds to: /visu/animation/E4 case
# Create animation for Plot 3D for 'pression' field of the the given MED file and dumps picture files in JPEG format %

import sys
import os
from paravistest import * 
from presentations import *
from pvsimple import *
import paravis

# import file
myParavis = paravis.myParavis

# for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "Animation/E4")

theFileName = datadir +  "TimeStamps_236.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "

myParavis.ImportFile(theFileName)
aProxy = GetActiveSource()
if aProxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"

print "Creating a Viewer.........................",
aView = GetRenderView()
reset_view(aView)
Render(aView)

if aView is None : print "Error"
else : print "OK"

# Plot 3D  creation
prs= Plot3DOnField(aProxy,EntityType.CELL,'pression' , 2)
prs.Visibility=1
aView.ResetCamera()
print "Creating an Animation.....................",
my_format = "jpeg"
print "Current format to save snapshots: ",my_format
# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

# Select only the current field:
aProxy.PointArrays.DeselectAll()
aProxy.CellArrays.DeselectAll()
aProxy.CellArrays = ['pression']
   
# Animation creation and saving into set of files into picturedir
scene = AnimateReader(aProxy,aView,picturedir+"E4_dom."+my_format)
nb_frames = len(scene.TimeKeeper.TimestepValues)

pics = os.listdir(picturedir) 
if len(pics) != nb_frames:
   print "FAILED!!! Number of made pictures is equal to ", len(pics), " instead of ", nb_frames
    
for pic in pics:
    os.remove(picturedir+pic)    
    
# Prepare animation  performance    
scene.PlayMode = 1 #  set RealTime mode for animation performance
# set period
scene.Duration = 30 # correspond to set the speed of animation in VISU 
scene.GoToFirst()
print "Animation.................................",
scene.Play()
scene.GoToFirst()
