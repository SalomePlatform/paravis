# This case corresponds to: /visu/3D_viewer/B2 case
# Create 3D Viewer and test set view properties for CutPlanes presentation
# Author: POLYANSKIKH VERA
from paravistest import *
from presentations import *
from pvsimple import *
import sys
import paravis
import time

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "3D_viewer/B0")

# Add path separator to the end of picture path if necessery
if not picturedir.endswith(os.sep):
    picturedir += os.sep

#import file
myParavis = paravis.myParavis

# Get view
my_view = GetRenderView()
reset_view(my_view)
Render(my_view)

# Split
a_view_r = CreateRenderView()
Delete(a_view_r)

# Split
a_view_l = CreateRenderView()
Delete(a_view_l)

# Split
a_view_t = CreateRenderView()
Delete(a_view_t)

# Split
a_view_b = CreateRenderView()
reset_view(a_view_b)
Render(a_view_b)
