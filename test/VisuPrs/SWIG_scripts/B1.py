# This case corresponds to: /visu/SWIG_scripts/B1 case

from time import sleep

from paravistest import datadir
from presentations import *
import paravis
import pvsimple


DELAY = 0.5

my_paravis = paravis.myParavis

# Get view
view = pvsimple.GetRenderView()
sleep(DELAY)

# Destroy the view
pvsimple.Delete(view)

# Create view and set background
view = pvsimple.CreateRenderView()
reset_view(view)

color = [0, 0.3, 1]
view.Background = color
pvsimple.Render()

# Import MED file
medFile = datadir + "pointe_236.med"
field_name = "fieldnodedouble"
entity = EntityType.NODE
timestamp = 1

my_paravis.ImportFile(medFile)
med_reader = pvsimple.GetActiveSource()

# Create scalar map
scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp)
pvsimple.Show(scalarmap.Input)

# Set view properties
print "view.CameraFocalPoint = [0, 0, 0]"
view.CameraFocalPoint = [0, 0, 0]
print "view.CameraParallelScale = 2"
view.CameraParallelScale = 2
print "pvsimple.ResetCamera(view)"
pvsimple.ResetCamera(view)

# Play with scalar bar
bar = get_bar()
lt = bar.LookupTable

range_min = lt.RGBPoints[0]
range_max = lt.RGBPoints[4]
delta = (range_max - range_min) / 2.0
nb_colors = lt.NumberOfTableValues
nb_colors = 64
lt.Discretize = 1
for i in xrange(2, nb_colors):
    lt.NumberOfTableValues = nb_colors
    x = range_min + delta * i / nb_colors
    y = range_max - delta * i / nb_colors
    lt.RGBPoints[0] = x
    lt.RGBPoints[4] = y
    pvsimple.Render(view)
    sleep(DELAY)

lt.RGBPoints[0] = range_min
lt.RGBPoints[4] = range_max

print "pvsimple.ResetCamera(view)"
pvsimple.ResetCamera(view)

# Create another view
view = pvsimple.CreateRenderView()
reset_view(view)

color = [0, 0.7, 0]
view.Background = color
pvsimple.Render(view)

displacement = 0.5
cutplanes = CutPlanesOnField(med_reader, entity, field_name, timestamp,
                             displacement=displacement)
print "CutPlanesOnField(...)"

display_only(cutplanes, view)
print "display_only(cutplanes, view)"

cam_pos = view.CameraPosition
cam_pos[0] = cam_pos[0] + 10
print "set view.CameraPosition"
cutplanes.Scale[0] = 3
cutplanes.Scale[1] = 10
pvsimple.Render(view)
sleep(DELAY)

pvsimple.ResetCamera(view)

slice_filter = cutplanes.Input
offset_vals = slice_filter.SliceOffsetValues
nb_planes = len(offset_vals)
cut_range = [offset_vals[0], offset_vals[-1]]

nb_planes = 30
#@MZN: FINISH ANGLES
for i in xrange(nb_planes, 1, -1):
    pos = get_positions(cut_range, i, displacement)
    slice_filter.SliceOffsetValues = pos
    pvsimple.Render(view)
    sleep(DELAY)

nb_planes = 10
cut_range = get_y_range(med_reader)
slice_filter.SliceType.Normal = [0, 1, 0]
for i in xrange(1, nb_planes):
    pos = get_positions(cut_range, i, displacement)
    slice_filter.SliceOffsetValues = pos
    pvsimple.Render(view)
    sleep(DELAY)

slice_filter.SliceType.Normal = [0, 0, 1]
slice_filter.UpdatePipeline()
print "pvsimple.ResetCamera(view)"
pvsimple.ResetCamera(view)

# Create one more view
view = pvsimple.CreateRenderView()
reset_view(view)

color = [1, 0.7, 0]
view.Background = color
pvsimple.Render(view)
sleep(DELAY)

isosurf = IsoSurfacesOnField(med_reader, entity, field_name, timestamp)
display_only(isosurf, view)
pvsimple.ResetCamera(view)
print "display_only(isosurf, view)"
sleep(DELAY)

contour = isosurf.Input
nb_surfaces = len(contour.Isosurfaces)
nb_surfaces = 32
scalar_range = get_data_range(med_reader, entity, field_name, cut_off=True)
for i in xrange(2, nb_surfaces):
    contours = get_contours(scalar_range, i)
    contour.Isosurfaces = contours
    pvsimple.Render(view)
    sleep(DELAY)

contour.Isosurfaces = get_contours(scalar_range, 10)
contour.UpdatePipeline()
print "pvsimple.ResetCamera(view)"
pvsimple.ResetCamera(view)

# Create one more view
view = pvsimple.CreateRenderView()
reset_view(view)

color = [0.7, 0.7, 0.7]
view.Background = color
pvsimple.Render(view)
sleep(DELAY)

cutlines = CutLinesOnField(med_reader, entity, field_name, timestamp,
                           orientation1=Orientation.ZX,
                           orientation2=Orientation.YZ)
display_only(cutlines, view)
pvsimple.ResetCamera(view)
print "display_only(cutlines, view)"
sleep(DELAY)

# Create one more view
view = pvsimple.CreateRenderView()
reset_view(view)

medFile = datadir + "TimeStamps_236.med"
field_name = "vitesse"
entity = EntityType.NODE
timestamp = 2

my_paravis.ImportFile(medFile)
med_reader = pvsimple.GetActiveSource()

isosurf = IsoSurfacesOnField(med_reader, entity, field_name, timestamp)
pvsimple.ResetCamera(view)

print "Start Animation"
pvsimple.AnimateReader(med_reader, view)
