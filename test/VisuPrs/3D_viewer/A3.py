# This case corresponds to: /visu/3D_viewer/A3 case
# Create 3D Viewer and test set view properties for Stream Lines presentation
# Author: POLYANSKIKH VERA
from paravistest import *
from presentations import *
from pvsimple import *
import sys
import paravis
import time

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "3D_viewer/A3")

# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

#import file
my_paravis = paravis.myParavis

# Get view
my_view = GetRenderView()
reset_view(my_view)
Render(my_view)

file_name = datadir + "Penta6_236.med"
print " --------------------------------- "
print "file ", file_name
print " --------------------------------- "

my_paravis.ImportFile(file_name)
proxy = GetActiveSource()
if proxy is None:
    raise RuntimeError("Error: can't import file.")
else:
    print "OK"

represents = [RepresentationType.POINTS, RepresentationType.WIREFRAME,\
RepresentationType.SURFACE, RepresentationType.VOLUME]
shrinks = [0, 1]
shadings = [0, 1]
opacities = [1.0, 0.5, 0.0]
linewidths = [1.0, 3.0, 10.0]
compare_prec = 0.00001

field_name = 'vectoriel_field'

print "\nCreating stream_lines......."
stream_lines = StreamLinesOnField(proxy, EntityType.CELL,
field_name, 1, is_colored=True)
stream_tracer = stream_lines.Input
print "stream_tracer:", stream_tracer
stream_tracer.Input = None
stream_tracer.InitialStepLength = 0.00940275
stream_tracer.MaximumStreamlineLength = 140.373
stream_tracer.MaximumStepLength = 0.5319
stream_tracer.MinimumStepLength = 0.5319
stream_tracer.UpdatePipeline()

if stream_lines is None:
    raise RuntimeError("Error!!! Presentation wasn't created...")

display_only(stream_lines, my_view)
reset_view(my_view)
Render(my_view)

print "\nChange Presentation Parameters..."

for reprCode in represents:
    repr = RepresentationType.get_name(reprCode)
    if reprCode == RepresentationType.VOLUME:
        is_good = call_and_check(stream_lines, "Representation", repr, 0)
        if is_good:
            msg = "VOLUME representation must be not available for streamlines"
            raise RuntimeError(msg)
    else:
        call_and_check(stream_lines, "Representation", repr, 1)

        for sha in shadings:
            setShaded(my_view, sha)
            call_and_check(stream_lines, "Shading", sha, 1)
            Render(my_view)
            time.sleep(1)

            for lwi in linewidths:
                call_and_check(stream_lines, "LineWidth", lwi, 1, compare_prec)
                # save picture in file
                # Construct image file name
                mask = "{folder}params_{repr}_any_{shading}_any_{lwi}.{ext}"
                pic_name = mask.format(folder=picturedir,
                                                repr=repr.replace(' ', '_'),
                                                shading=sha,
                                                lwi=lwi,
                                                ext=pictureext)
                # Show and record the presentation
                process_prs_for_test(stream_lines, my_view, pic_name)

for opa in opacities:
    call_and_check(stream_lines, "Opacity", opa, 1, compare_prec)
