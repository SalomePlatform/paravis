# Copyright (C) 2010-2013  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# This case corresponds to: /visu/StreamLines/B3 case
# Create Stream Lines for all fields of each MED file from the given MED files list

import sys

from paravistest import datadir, pictureext, get_picture_dir
from presentations import CreatePrsForFile, PrsTypeEnum
import paravis

myParavis = paravis.myParavis

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "StreamLines/B3")

# Create presentations
files = ["fra", "TimeStamps", "pointe", "Fields_group3D", "Hexa8", "Penta6", "Quad4", "Tetra4", "Tria3", "clo", "carre_en_quad4_seg2", "carre_en_quad4_seg2_fields", "cube_hexa8_quad4"]

for item in files:
    file = datadir + item + ".med"
    print " --------------------------------- "
    print "file ", file
    print "\nCreatePrsForFile..."
    print "BREAKPOINT_1"
    CreatePrsForFile(myParavis, file, [PrsTypeEnum.STREAMLINES], picturedir, pictureext)
