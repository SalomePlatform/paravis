# Copyright (C) 2017-2023  CEA, EDF
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

from os import path, remove
#### import the simple module from the paraview
from paraview.simple import *
#from pvsimple import *
from vtk.util import numpy_support
import numpy as np

from medcoupling import *
import MEDLoader as ml
from MEDLoader import *

import inspect
data_dir = os.path.dirname(inspect.getfile(lambda: None))

def MyAssert(clue):
    if not clue:
        raise RuntimeError("Assertion failed !")


def test_fields(result, ref, field_name):
    """
    Test fields content of result from VoroGauss filter
    """
    ds0 = servermanager.Fetch(result)
    block = ds0.GetBlock(0)

    data = numpy_support.vtk_to_numpy(block.GetCellData().GetArray(field_name))

    MyAssert(np.allclose(data, ref))


###
# Test of PG_3D.med
###
# create a new 'MED Reader'
file_name = os.path.join(data_dir, "PG_3D.med")
pG_3Dmed = MEDReader(registrationName='PG_3D.med', FileNames=file_name)
pG_3Dmed.FieldsStatus = ['TS0/Extruded/ComSup0/Extruded@@][@@P0',
                         'TS0/Extruded/ComSup0/MyFieldPG@@][@@GAUSS']
print("Testing {}".format(file_name))
pG_3Dmed.UpdatePipeline()

fields = [('MyFieldPG_avg',
           list(range(3, 445, 7))
          ),
          ('MyFieldPG_min',
           list(range(0, 442, 7))
          ),
          ('MyFieldPG_max',
           list(range(6, 448, 7))
          )
         ]

# create a new 'ELGA field To Surface'
eELGAfieldToSurfacecellaveraged1 = ELGAfieldToSurfacecellaveraged(registrationName='ELGAfieldToSurface2', Input=pG_3Dmed)
eELGAfieldToSurfacecellaveraged1.UpdatePipeline()

for field_name, ref in fields:
    print(" ~> Field check for field {}".format(field_name))
    test_fields(eELGAfieldToSurfacecellaveraged1, np.array(ref), field_name)

