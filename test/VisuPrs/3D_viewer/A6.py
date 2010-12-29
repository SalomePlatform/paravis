# This case corresponds to: /visu/3D_viewer/A6 case
# Create 3D Viewer and test set view properties for Scalar Map on Deformed Shape presentation
# Author:       POLYANSKIKH VERA
from paravistest import * 
from presentations import *
from pvsimple import *
import sys
import paravis
import time
      

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "3D_viewer/A6")

                    
# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

#import file
myParavis = paravis.myParavis

# Get view
my_view = GetRenderView()
reset_view(my_view)
Render(my_view)

theFileName = datadir +  "cube_hexa8_quad4_236.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "

myParavis.ImportFile(theFileName)
proxy = GetActiveSource()
if proxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"

represents = [RepresentationType.POINTS,RepresentationType.WIREFRAME,
                                RepresentationType.SURFACE,RepresentationType.VOLUME]
shrinks    = [0, 1]
shadings   = [0, 1]
opacities  = [1.0, 0.5, 0.0]
linewidths = [1.0, 3.0, 10.0]
compare_prec = 0.00001
shrink_filter = None
shrinked_sm_on_ds = None

field_name = 'fieldcelldouble'

print "\nCreating scalar map on deformed shape.......",
sm_on_ds= DeformedShapeAndScalarMapOnField(proxy,EntityType.CELL,field_name, 1,scale_factor=0.2)
if sm_on_ds is None : raise RuntimeError, "Error!!! Presentation wasn't created..."

display_only(sm_on_ds,my_view)
reset_view(my_view)
Render(my_view)

print "\nChange Presentation Parameters..."


for reprCode in represents:
    repr  = RepresentationType.get_name(reprCode) 
    call_and_check(sm_on_ds,"Representation", repr, 1)
    for shr in shrinks:
        if shr > 0 and reprCode != RepresentationType.POINTS :
            if shrinked_sm_on_ds is None:
                sm_on_ds.Visibility = 1
                shrink_filter  = Shrink(sm_on_ds.Input)
                shrink_filter.ShrinkFactor = 0.8
                shrink_filter.UpdatePipeline()
                shrinked_sm_on_ds  = GetRepresentation(shrink_filter)
                shrinked_sm_on_ds.ColorArrayName = sm_on_ds.ColorArrayName   
                shrinked_sm_on_ds.LookupTable = sm_on_ds.LookupTable
                shrinked_sm_on_ds.ColorAttributeType = sm_on_ds.ColorAttributeType          
            sm_on_ds.Visibility = 0
            shrinked_sm_on_ds.Representation = sm_on_ds.Representation                
            shape_to_show =  shrinked_sm_on_ds   
        else:             
            hide_all(my_view)
            shape_to_show =  sm_on_ds   
        shape_to_show.Visibility = 1            
        Render(my_view)             
                
        for sha in shadings:
            setShaded(my_view, sha)
            call_and_check(shape_to_show,"Shading", sha, 1)        
            Render(my_view)    
                
            for opa in opacities:
                call_and_check(shape_to_show,"Opacity", opa, 1, compare_prec)
    
                for lwi in linewidths:
                    call_and_check(shape_to_show,"LineWidth", lwi, 1, compare_prec)
                        
                    time.sleep(1)        
                    # save picture in file
                    # Construct image file name
                    pic_name = "{folder}params_{repr}_{shrink}_{shading}_{opa}_{lwi}.{ext}".format(folder=picturedir,
                                                                                                                            repr  = repr.replace(' ','_'),
                                                                                                                            shrink=shr,
                                                                                                                            shading = sha,
                                                                                                                            opa = opa,
                                                                                                                            lwi = lwi,
                                                                                                                            ext=pictureext)        
                    # Show and record the presentation
                    process_prs_for_test(shape_to_show, my_view, pic_name)           
                    pass
                pass
            pass
        pass
    pass                    
            
