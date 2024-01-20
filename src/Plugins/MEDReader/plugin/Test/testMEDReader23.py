#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2021-2024  CEA, EDF
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
# Author : Anthony Geay (EDF R&D)

__doc__  = """
Non regression test of EDF24514. GlobalNodeIds present in MED file whereas it does not appear into output DataSet of MEDReader.
"""

from paraview.simple import *
import medcoupling as mc
from vtk.util import numpy_support

def MyAssert(clue):
  if not clue:
    raise RuntimeError("Assertion failed !")

fname = 'testMEDReader23.med'
ids = mc.DataArrayInt(40) ; ids.iota(100) # array used to define GlobalNodeIds. Used to be checked
coo = mc.DataArrayDouble(40) ; coo.iota()
coo = coo.changeNbOfComponents(3,0.)

def generateCase(fname,globalNodeIds,coordinates):
  m = mc.MEDCouplingUMesh.Build1DMeshFromCoords(coordinates)
  m.setName("mesh")
  mm = mc.MEDFileUMesh()
  mm[0] = m
  mm.setGlobalNumFieldAtLevel(1,globalNodeIds)
  mm.write(fname,2)

generateCase(fname,ids,coo)
myMedReader = MEDReader(registrationName = fname, FileNames = [fname])
myMedReader.FieldsStatus = ['TS0/mesh/ComSup0/mesh@@][@@P0']
myMedReader.UpdatePipeline()
# first important testing here
MyAssert(   [myMedReader.PointData.GetArray(i).GetName() for i in range(myMedReader.PointData.GetNumberOfArrays())] == ['GlobalNodeIds','vtkGhostType']    )
ReadUnstructuredGrid = servermanager.Fetch(myMedReader).GetBlock(0)
numpy_support.vtk_to_numpy( ReadUnstructuredGrid.GetPointData().GetArray('GlobalNodeIds') )
# check match of coordinates written in testMEDReader23.med file and its representation
MyAssert( mc.DataArrayInt(numpy_support.vtk_to_numpy( ReadUnstructuredGrid.GetPointData().GetArray('GlobalNodeIds') ) ).isEqual(ids) )
MyAssert( mc.DataArrayDouble(numpy_support.vtk_to_numpy(ReadUnstructuredGrid.GetPoints().GetData())).isEqual(coo,1e-12) )
