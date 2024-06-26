#  -*- coding: iso-8859-1 -*-
# Copyright (C) 2007-2024  CEA, EDF
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
# Author : Anthony Geay

import os
import sys

from medcoupling import *
from paraview.simple import *
from MEDReaderHelper import WriteInTmpDir,RetriveBaseLine

def GenerateCase():
  """ This use case is a mesh containing a large number of orphan cells (cells having no field lying on them)
  """
  fname="testMEDReader2.med"
  #########
  arrX=DataArrayDouble(7) ; arrX.iota()
  arrY=DataArrayDouble(7) ; arrY.iota()
  arrZ=DataArrayDouble(7) ; arrZ.iota()
  m=MEDCouplingCMesh()
  m.setCoords(arrX,arrY,arrZ)
  m=m.buildUnstructured() ; m.setName("mesh")
  tmp=m[3*36:4*36]
  tmp=tmp.buildDescendingConnectivity()[0]
  nodeIds=tmp.findNodesOnPlane([0.,0.,3.],[0.,0.,1.],1e-12)
  cellIds=tmp.getCellIdsLyingOnNodes(nodeIds,True)
  m1=tmp[cellIds]
  mm=MEDFileUMesh()
  mm.setMeshAtLevel(0,m)
  mm.setMeshAtLevel(-1,m1)
  mm.write(fname,2)
  #
  pfl=DataArrayInt([7,8,9,10,13,14,15,16,19,20,21,22,25,26,27,28]) ; pfl.setName("pfl")
  f=MEDCouplingFieldDouble(ON_CELLS) ; f.setName("ACellField")
  arr=DataArrayDouble(16) ; arr.iota()
  arr2=arr.deepCopy() ; arr2.reverse()
  arr=DataArrayDouble.Meld(arr,arr2) ; arr.setInfoOnComponents(["aa","bbb"])
  f.setArray(arr)
  f1ts=MEDFileField1TS()
  f1ts.setFieldProfile(f,mm,-1,pfl)
  f1ts.write(fname,0)
  return fname

@WriteInTmpDir
def test(baseline_file):
  fname = GenerateCase()
  ################### MED write is done -> Go to MEDReader
  testMEDReader1=MEDReader(FileNames=[fname],registrationName='testMEDReader2.med')
  testMEDReader1.FieldsStatus=['TS0/mesh/ComSup0/ACellField@@][@@P0']
  testMEDReader2=MEDReader(FileNames=[fname],registrationName='testMEDReader2_bis.med')
  testMEDReader2.FieldsStatus=['TS0/mesh/ComSup1/mesh@@][@@P0']
  GroupDatasets1=GroupDatasets(Input=[testMEDReader1,testMEDReader2])
  #GroupDatasets1.BlockNames = ['testMEDReader2.med', 'testMEDReader2_bis.med']

  Clip1 = Clip(ClipType="Plane",Input=GroupDatasets1)
  Clip1.Scalars=['FamilyIdCell']
  Clip1.ClipType.Origin=[3.0, 3.0, 3.0]
  Clip1.Invert=1
  Clip1.ClipType.Normal=[0.9255623174457069, 0.0027407477590518157, 0.378585373233375]
  Clip1.Scalars=['CELLS']

  DataRepresentation4 = Show()
  DataRepresentation4.EdgeColor = [0.0, 0.0, 0.5000076295109483]
  DataRepresentation4.SelectionCellFieldDataArrayName = 'ACellField'
  DataRepresentation4.ScalarOpacityUnitDistance = 1.61104723630366
  DataRepresentation4.BlockSelectors = ['/Root']
  DataRepresentation4.ScaleFactor = 0.6000000000000001
  DataRepresentation4.Visibility = 1
  DataRepresentation4.Representation = 'Wireframe'
  ExtractBlock1 = ExtractBlock(Input=Clip1)
  ExtractBlock1.Selectors = ['/Root/testMEDReader2med']

  DataRepresentation5 = Show()
  DataRepresentation5.EdgeColor = [0.0, 0.0, 0.5000076295109483]
  DataRepresentation5.SelectionCellFieldDataArrayName = 'FamilyIdCell'
  DataRepresentation5.ScaleFactor = 0.6
  a2_ACellField_PVLookupTable=GetLookupTableForArray( "ACellField", 2, RGBPoints=[10.63014581273465, 0.23, 0.299, 0.754, 15.0, 0.706, 0.016, 0.15], VectorMode='Magnitude', NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0, AllowDuplicateScalars=1 )
  a2_ACellField_PiecewiseFunction=CreatePiecewiseFunction( Points=[0.0, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0] )
  a2_ACellField_PVLookupTable.ScalarOpacityFunction = a2_ACellField_PiecewiseFunction
  DataRepresentation5.ScalarOpacityFunction = a2_ACellField_PiecewiseFunction
  DataRepresentation5.LookupTable = a2_ACellField_PVLookupTable
  DataRepresentation5.ColorArrayName = ("CELLS", "ACellField")

  if '-D' not in sys.argv:
    RenderView1 = GetRenderView()
    RenderView1.CenterOfRotation = [3.0, 3.0, 3.0]
    RenderView1.CameraViewUp = [-0.03886073885859842, 0.48373409998193495, 0.8743518533691291]
    RenderView1.CameraPosition = [7.351939549758929, -5.688193007926853, 8.000155023042788]
    RenderView1.CameraFocalPoint = [2.9999999999999996, 2.9999999999999987, 2.9999999999999982]

    RenderView1.ViewSize =[300,300]
    Render()

    #WriteImage(outImgName)

    # compare with baseline image
    import vtk.test.Testing
    from vtk.util.misc import vtkGetTempDir
    vtk.test.Testing.VTK_TEMP_DIR = vtk.util.misc.vtkGetTempDir()
    vtk.test.Testing.compareImage(GetActiveView().GetRenderWindow(), baseline_file,
                                                                threshold=1)
    vtk.test.Testing.interact()

if __name__ == "__main__":
  outImgName="testMEDReader2.png"
  baseline_file = RetriveBaseLine(outImgName)
  test(baseline_file)
  pass
