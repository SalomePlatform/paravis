# Copyright (C) 2010-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

# This case corresponds to: /visu/imps/B1 case

from paravistest import datadir, texturesdir
from presentations import *
import pvsimple

# Import MED file
file_path = datadir + "fra.med"
pvsimple.OpenDataFile(file_path)
med_reader = pvsimple.GetActiveSource()

entity = EntityType.NODE
field_name = "VITESSE"
timestamp_id = 1

# Create Scalar Map
scalarmap = ScalarMapOnField(med_reader, entity, field_name, timestamp_id)

scalarmap.Visibility = 1
reset_view()

# Set representation type to Point Sprite
scalarmap.Representation = 'Point Sprite'

# Set texture
scalarmap.RenderMode = 'Texture'
# COMMENTED OUT! Currently this does not work as Point Sprite ParaView plugin is not correctly wrapped to Python.
# As soon as problem is fixed, below code probably need to be modified, but it should be something similar.
#import vtk
#texture = vtk.vtkTexture()
#pngReader = vtk.vtkPNGReader()
#pngReader.SetFileName(os.path.join(texturesdir, "texture1.png"))
#texture.SetInputConnection(pngReader.GetOutputPort())
#texture.InterpolateOn()
#scalarmap.Texture = texture

pvsimple.Render()
