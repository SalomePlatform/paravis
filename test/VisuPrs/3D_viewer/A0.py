#This case corresponds to: /visu/3D_viewer/A0 case
#%Created 3D Viewer%
#Created:      25/11/2010
#Author:       POLYANSKIKH VERA
from paravistest import * 
from presentations import *
from pvsimple import *
import sys

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "3D_viewer/A0")

# Get view
my_view = GetRenderView()
hide_all(my_view)
##
##my_view.RestoreViewParams("SAVE");
##my_view.SaveViewParams("SAVE");
##
##print "Default Title of view        ... ",my_view.GetTitle()
##my_view.SetTitle('   ')
##my_view.SetTitle("Viewer first")
##print "Title of view                ... ",my_view.GetTitle()
##
##my_view.SetView(VISU.View3D.FRONT)
##my_view.SaveViewParams("tmp1")

##my_view.SetView(VISU.View3D.BACK)
##my_view.SetView(VISU.View3D.LEFT)
##my_view.SetView(VISU.View3D.RIGHT)
##my_view.SetView(VISU.View3D.TOP)
##my_view.SetView(VISU.View3D.BOTTOM)



def_focal_point = my_view.CameraFocalPoint
print "Default focal point          ... ",def_focal_point
def_focal_point = [4.4,5.9,9.567]

my_view.CameraFocalPoint = def_focal_point

###my_view.SaveViewParams("tmp3");
print "Focal point                  ... ",my_view.CameraFocalPoint
def_focal_point = [4.4,5.9,-9.567]
my_view.CameraFocalPoint = def_focal_point
print "Focal point                  ... ",my_view.CameraFocalPoint

def_focal_point = [4.4,-5.9,9.567]
my_view.CameraFocalPoint = def_focal_point
print "Focal point                  ... ",my_view.CameraFocalPoint
def_focal_point = [-4.4,5.9,9.567]
my_view.CameraFocalPoint = def_focal_point
print "Focal point                  ... ",my_view.CameraFocalPoint
def_focal_point = [-4.4,-5.9,-9.567]
my_view.CameraFocalPoint = def_focal_point
print "Focal point                  ... ",my_view.CameraFocalPoint
def_focal_point = [4.4,-5.9,-9.567]
my_view.CameraFocalPoint = def_focal_point
print "Focal point                  ... ",my_view.CameraFocalPoint
def_focal_point = [-4.4,5.9,-9.567]
my_view.CameraFocalPoint = def_focal_point
print "Focal point                  ... ",my_view.CameraFocalPoint
def_focal_point = [-4.4,-5.9,9.567]
my_view.CameraFocalPoint = def_focal_point
print "Focal point                  ... ",my_view.CameraFocalPoint

my_view.CameraFocalPoint = [0,0,0]

def_scale = my_view.CameraParallelScale
print "Default parallel scale       ... ",def_scale

###my_view.SaveViewParams("tmp4");
my_view.CameraParallelScale = -2 ;
my_view.CameraParallelScale = 0;
###my_view.RestoreViewParams("tmp4");
my_view.CameraParallelScale = 2;
print "Parallel scale               ... ",my_view.CameraParallelScale
##my_view.SaveViewParams("SAVE5");

def_point = my_view.CameraPosition;
print "Default point of view        ... ",def_point
point = [2,2,2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition
point = [2,2,-2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition
point = [2,-2,2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition
point = [-2,2,2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition
point = [-2,-2,-2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition
point = [2,-2,-2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition
point = [-2,2,-2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition
point = [-2,-2,2]
my_view.CameraPosition = point
print "Point of view                ... ",my_view.CameraPosition

def_point[0] = def_point[0] + 10;
my_view.CameraPosition = def_point;
my_view.CameraPosition = [9.9997100000000003, 0.0, -1279.8499999999999]

def_line = my_view.CameraViewUp
print "Default vertical line of view... ",def_line
line = [1,2,5]
my_view.CameraViewUp = line
###my_view.SaveViewParams("tmp2")
print "Vertical line of view        ... ",my_view.CameraViewUp
line = [1,2,-5]
my_view.CameraViewUp = line
print "Vertical line of view        ... ",my_view.CameraViewUp

###my_view.RestoreViewParams("tmp2")
line = [1,-2,5]
my_view.CameraViewUp = line
print "Vertical line of view        ... ",my_view.CameraViewUp
line = [-1,2,5]
my_view.CameraViewUp = line
print "Vertical line of view        ... ",my_view.CameraViewUp
line = [-1,-2,-5]
my_view.CameraViewUp = line
print "Vertical line of view        ... ",my_view.CameraViewUp
line = [1,-2,-5]
my_view.CameraViewUp = line
print "Vertical line of view        ... ",my_view.CameraViewUp
line = [-1,2,-5]
my_view.CameraViewUp = line
print "Vertical line of view        ... ",my_view.CameraViewUp
line = [-1,-2,5]
my_view.CameraViewUp = line
print "Vertical line of view        ... ",my_view.CameraViewUp
line = [0,1,0]
my_view.CameraViewUp = line
###my_view.SaveViewParams("SAVE3");

##my_view.RemoveScale()
##my_view.ScaleView(VISU.View3D.YAxis,10.0)
##my_view.SaveViewParams("tmp5");
##my_view.RemoveScale()
##my_view.ScaleView(VISU.View3D.YAxis,-10.0)
##my_view.ScaleView(VISU.View3D.YAxis,0)
##my_view.RestoreViewParams("tmp5");
##my_view.ScaleView(VISU.View3D.ZAxis,4.0)
##
##my_view.ScaleView(VISU.View3D.YAxis,10.0);
##my_view.ScaleView(VISU.View3D.XAxis,3.0);
##my_view.SaveViewParams("SAVE5");

def_color = my_view.Background
print "Default View Background color...  (",def_color[0],",",def_color[1],",",def_color[2],")"
color = [0.0,0.3,1.0]
my_view.Background = color;
def_color= my_view.Background
print "View Background color        ...  (",def_color[0],",",def_color[1],",",def_color[2],")"

Render(my_view)

###save Viewer parameters
##my_view.RemoveScale();
#my_view.ResetCamera();
##my_view.SaveViewParams("SAVE1");


##my_view.ScaleView(VISU.View3D.YAxis,30.0);###my_view.RestoreViewParams("SAVE1")

Render(my_view)
###my_view.RestoreViewParams("SAVE1")


##
##my_view.Minimize()
##my_view.Restore()
##my_view.Maximize()
##my_view.Restore()

print "eND focal point          ... ",my_view.CameraFocalPoint
print "End   point of view        ... ", my_view.CameraPosition
print "End   view up        ... ", my_view.CameraViewUp

# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

# Construct image file name
pic_name = "{folder}{mesh}.{ext}".format(folder=picturedir,
                                                                        mesh='A0_3D_viewer',
                                                                        ext=pictureext)
    
process_prs_for_test('A0_3D_viewer', my_view, pic_name)

hide_all(my_view)
display_all(my_view)
