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
  """ This test focuses on GenerateVector part.
  """
  fname="testMEDReader10.med"
  ##############
  coo=DataArrayDouble([(0.,0.,0.),(1.,0.,0.),(2.,0.,0.),(3.,0.,0.),(0.,1.,0.),(1.,1.,0.),(2.,1.,0.),(3.,1.,0.),(0.,0.,1.),(1.,0.,1.),(2.,0.,1.),(3.,0.,1.),(0.,1.,1.),(1.,1.,1.),(2.,1.,1.),(3.,1.,1.)])
  m=MEDCouplingUMesh("myMesh",3) ; m.setCoords(coo)
  m.allocateCells()
  m.insertNextCell(NORM_HEXA8,[0,4,5,1,8,12,13,9]) ; m.insertNextCell(NORM_HEXA8,[1,5,6,2,9,13,14,10]) ; m.insertNextCell(NORM_HEXA8,[2,6,7,3,10,14,15,11])
  f0=MEDCouplingFieldDouble(ON_NODES) ; f0.setName("f0NbComp1") ; f0.setMesh(m) ;
  arr=DataArrayDouble(8) ; arr.iota() ; arr=DataArrayDouble.Aggregate(arr,arr+8) ; arr.setInfoOnComponents(["1st"])
  f0.setArray(arr)
  f1=MEDCouplingFieldDouble(ON_NODES) ; f1.setName("f1NbComp2") ; f1.setMesh(m) ;
  arr=DataArrayDouble([(-1.,-1.),(0.,0.5),(0.,0.5),(1.,-1.),(-1.,1.),(0.,-0.5),(0.,-0.5),(1.,1.)]) ; arr=DataArrayDouble.Aggregate(arr,arr) ; arr.setInfoOnComponents(["1st","2nd"])
  f1.setArray(arr)
  f2=MEDCouplingFieldDouble(ON_NODES) ; f2.setName("f2NbComp3") ; f2.setMesh(m) ;
  arr=DataArrayDouble([(-1.,-1.,-1.),(0.,0.5,0.),(0.,0.5,0.),(1.,-1.,-1.),(-1.,1.,-1.),(0.,-0.5,0.),(0.,-0.5,0.),(1.,1.,-1)]) ; arr2=arr[:] ; arr2[[0,3,4,7],2]=1.
  arr=DataArrayDouble.Aggregate(arr,arr2) ; arr.setInfoOnComponents(["1st","2nd","3rd"])
  f2.setArray(arr)
  f3=MEDCouplingFieldDouble(ON_NODES) ; f3.setName("f3NbComp4") ; f3.setMesh(m) ;
  arr=DataArrayDouble([(-1.,-1.,-1.),(0.,0.5,0.),(0.,0.5,0.),(1.,-1.,-1.),(-1.,1.,-1.),(0.,-0.5,0.),(0.,-0.5,0.),(1.,1.,-1)]) ; arr2=arr[:] ; arr2[[0,3,4,7],2]=1.
  arr=DataArrayDouble.Aggregate(arr,arr2) ; arr3=DataArrayDouble(16) ; arr3.iota() ; arr=DataArrayDouble.Meld([arr,arr3])
  arr.setInfoOnComponents(["1st","2nd","3rd","4th"])
  f3.setArray(arr)
  #
  WriteMesh(fname,m,True)
  WriteFieldUsingAlreadyWrittenMesh(fname,f0)
  WriteFieldUsingAlreadyWrittenMesh(fname,f1)
  WriteFieldUsingAlreadyWrittenMesh(fname,f2)
  WriteFieldUsingAlreadyWrittenMesh(fname,f3)
  return fname


@WriteInTmpDir
def test(baseline_file):
  fname = GenerateCase()
  ################### MED write is done -> Go to MEDReader
  myMedReader=MEDReader(FileNames=[fname])
  keys=myMedReader.GetProperty("FieldsTreeInfo")[::2]
  # list all the names of arrays that can be seen (including their spatial discretization)
  arr_name_with_dis=[elt.split("/")[-1] for elt in keys]
  # list all the names of arrays (Equal to those in the MED File)
  arr_name=[elt.split(myMedReader.GetProperty("Separator").GetData())[0] for elt in arr_name_with_dis]
  myMedReader.FieldsStatus=keys
  myMedReader.VectorsProperty=1

  if '-D' not in sys.argv:
    RenderView1=GetRenderView()
    DataRepresentation1 = Show()
    DataRepresentation1.EdgeColor = [0.0, 0.0, 0.5]
    DataRepresentation1.SelectionPointFieldDataArrayName='f0NbComp1'
    DataRepresentation1.ScalarOpacityUnitDistance = 2.299619191183727
    #DataRepresentation1.ExtractedBlockIndex = 1
    DataRepresentation1.ScaleFactor = 0.3
    RenderView1.CenterOfRotation = [1.5, 0.5, 0.5]
    RenderView1.CameraPosition = [1.5, 0.5, 6.907227082229696]
    RenderView1.CameraFocalPoint = [1.5, 0.5, 0.5]
    RenderView1.CameraParallelScale = 1.6583123951777
    DataRepresentation1.Representation = 'Wireframe'
    #
    wbv=WarpByVector(Input=myMedReader)
    wbv.ScaleFactor=0.1
    wbv.Vectors=['POINTS','f3NbComp4_Vector']
    print( list(wbv.PointData.keys()) )
    assert(list(wbv.PointData.keys())==['f0NbComp1','f1NbComp2','f1NbComp2_Vector','f2NbComp3','f3NbComp4','f3NbComp4_Vector','vtkGhostType'])
    #
    DataRepresentation2 = Show()
    DataRepresentation2.EdgeColor = [0.0, 0.0, 0.5000076295109483]
    DataRepresentation2.SelectionPointFieldDataArrayName = 'f0NbComp1'
    DataRepresentation2.SelectionCellFieldDataArrayName = 'FamilyIdCell'
    DataRepresentation2.ScalarOpacityUnitDistance = 4.546673931685981
    #DataRepresentation2.ExtractedBlockIndex = 1
    DataRepresentation2.ScaleFactor = 0.5
    DataRepresentation1.Visibility = 0
    DataRepresentation1.ColorArrayName = ('POINT_DATA', '')
    DataRepresentation1.Visibility = 1
    a4_f3NbComp4_PVLookupTable = GetLookupTableForArray( "f3NbComp4", 4, RGBPoints=[1.118033988749895, 0.23, 0.299, 0.754, 8.108851429645696, 0.865, 0.865, 0.865, 15.0996688705415, 0.706, 0.016, 0.15], VectorMode='Magnitude', NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0 )
    a4_f3NbComp4_PiecewiseFunction = CreatePiecewiseFunction( Points=[1.118033988749895, 0.0, 0.5, 0.0, 15.0996688705415, 1.0, 0.5, 0.0] )
    DataRepresentation1.ScalarOpacityFunction = a4_f3NbComp4_PiecewiseFunction
    DataRepresentation1.LookupTable = a4_f3NbComp4_PVLookupTable
    a4_f3NbComp4_PVLookupTable.ScalarOpacityFunction = a4_f3NbComp4_PiecewiseFunction
    DataRepresentation2.ScalarOpacityFunction = a4_f3NbComp4_PiecewiseFunction
    DataRepresentation2.ColorArrayName = ('POINT_DATA', 'f3NbComp4')
    DataRepresentation2.LookupTable = a4_f3NbComp4_PVLookupTable
    RenderView1.CameraViewUp = [0.32310237626907823, -0.28721735329781684, -0.901726702728644]
    RenderView1.CameraPosition = [0.17151044563787257, 6.31903262423349, -1.8294921560963984]
    RenderView1.CameraFocalPoint = [1.4999999999999996, 0.49999999999999944, 0.4999999999999996]
    #
    RenderView1.ViewSize =[300,300]
    Render()

    # compare with baseline image
    import vtk.test.Testing
    from vtk.util.misc import vtkGetTempDir
    vtk.test.Testing.VTK_TEMP_DIR = vtk.util.misc.vtkGetTempDir()
    vtk.test.Testing.compareImage(GetActiveView().GetRenderWindow(), baseline_file,
                                                                threshold=1)
    vtk.test.Testing.interact()

if __name__ == "__main__":
  outImgName="testMEDReader10.png"
  baseline_file = RetriveBaseLine(outImgName)
  test(baseline_file)
  pass
