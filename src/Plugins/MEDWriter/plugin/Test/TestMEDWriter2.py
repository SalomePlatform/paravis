# Copyright (C) 2025-2026  CEA, EDF
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

"""
[EDF32592] : Check write managment of VTK_VOXEL by MEDWriter. VTK_VOXELs araise after a threshold on cartesian mesh.
"""

from medcoupling import *
from pathlib import Path
import vtk
from tempfile import TemporaryDirectory

fname = "testCMesh.med"

arr=DataArrayDouble(5) ; arr.iota()
m=MEDCouplingCMesh() ; m.setCoords(arr,arr,arr)
m.setName("mesh")
f = MEDCouplingFieldDouble(ON_CELLS)
f.setMesh(m)
f.setName("field")
arr = DataArrayDouble( f.getNumberOfTuplesExpected() )
arr.iota()
f.setArray(arr)
#tmpdirname = "."
with TemporaryDirectory() as tmpdirname:
    zeFileName = "{}".format( Path( tmpdirname ) / fname )
    f.write( zeFileName )
    from paraview.simple import *
    outputFileName = "{}".format(Path(zeFileName).parent / "testCMesh2.med")
    testCMeshmed = MEDReader(registrationName=Path(zeFileName).name, FileNames=[ "{}".format(Path(zeFileName)) ])
    threshold1 = Threshold(registrationName='Threshold1', Input=testCMeshmed)
    threshold1.Scalars = ['CELLS', 'field']
    threshold1.UpperThreshold = 31.0
    SaveData( outputFileName , proxy=threshold1)
    thresData = servermanager.Fetch(threshold1)
    assert( thresData.GetBlock(0).GetCellType(0) == vtk.VTK_VOXEL ) # check that threshold1 contains VTK_VOXEL
    mm = MEDFileMesh.New( outputFileName )
    mToTest = mm[0]
    mToTest.getCoords().setInfoOnComponents(["","",""])
    mInit = m.buildUnstructured()
    mInit2 = mInit[0:32:1]
    mInit2.zipCoords()
    assert( mInit2.getCoords().isEqual(mToTest.getCoords(),1e-12) )
    assert( mToTest.isEqual(mInit2,1e-12) )
