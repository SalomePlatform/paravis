# This case corresponds to: /visu/3D_viewer/A2 case
# Create 3D Viewer and test set view properties for Vectors presentation
# Author: POLYANSKIKH VERA
from paravistest import * 
from presentations import *
from pvsimple import *
import sys
import paravis
import time


    
# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "3D_viewer/A2")

            
# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

#import file
myParavis = paravis.myParavis

# Get view
my_view = GetRenderView()
reset_view(my_view)
Render(my_view)

theFileName = datadir +  "Penta6_236.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "

myParavis.ImportFile(theFileName)
proxy = GetActiveSource()
if proxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"

represents = [RepresentationType.POINTS,RepresentationType.WIREFRAME,\
RepresentationType.SURFACE,RepresentationType.VOLUME]
shrinks    = [0, 1]
shadings   = [0, 1]
opacities  = [1.0, 0.5, 0.0]
linewidths = [1.0, 3.0, 10.0]
compare_prec = 0.00001

field_name = 'vectoriel_field'


print "\nCreating Vectors.......",
vectors= VectorsOnField(proxy,EntityType.CELL,field_name, 1,is_colored=True,glyph_type = "Cone")
if vectors is None : raise RuntimeError, "Error!!! Presentation wasn't created..."

display_only(vectors,my_view)
reset_view(my_view)
Render(my_view)

print "\nChange Presentation Parameters..."

for reprCode in represents:
    repr  = RepresentationType.get_name(reprCode) 
    if reprCode == RepresentationType.VOLUME:
        is_good = call_and_check(vectors,"Representation", repr, 0)
        if is_good:
            raise RuntimeError, "VOLUME representation must be not available for Vectors"
    else:
        call_and_check(vectors,"Representation", repr, 1)

        for sha in shadings:
            my_view.LightSwitch = sha
            call_and_check(vectors,"Shading", sha, 1)            
            Render(my_view)
            time.sleep(1)

            # save picture in file
            # Construct image file name
            pic_name = "{folder}params_{repr}_any_{shading}_any_any.{ext}".format(folder=picturedir,
                                                                                   repr  = repr.replace(' ','_'),
                                                                                   shading = sha,
                                                                                    ext=pictureext)        
            # Show and record the presentation
            process_prs_for_test(vectors, my_view, pic_name)
             

for opa in opacities:
    call_and_check(vectors,"Opacity", opa, 1, compare_prec)

for lwi in linewidths:
    call_and_check(vectors,"LineWidth", lwi, 1, compare_prec)
