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
# Author : Anthony Geay (EDF R&D)

from medcoupling import *
from paraview.simple import *
from paraview import servermanager
from MEDReaderHelper import WriteInTmpDir,RetriveBaseLine

paraview.simple._DisableFirstRenderCameraReset()

def GenerateCase():
    """ This test is a non regression test of EDF8662 : This bug revealed that ELNOfieldToSurface and ELNOfieldToPointGaussian do not behave correctly after the call of ExtractGroup"""

    fname="testMEDReader16.med"

    arr=DataArrayDouble([0,1,2])
    m=MEDCouplingCMesh() ; m.setCoords(arr,arr) ; m=m.buildUnstructured() ; m.setName("Mesh")
    mm=MEDFileUMesh() ; mm.setMeshAtLevel(0,m)
    grp0=DataArrayInt([0,1]) ; grp0.setName("grp0")
    grp1=DataArrayInt([2,3]) ; grp1.setName("grp1")
    grp2=DataArrayInt([1,2]) ; grp2.setName("grp2")
    grp3=DataArrayInt([0,1,2,3]) ; grp3.setName("grp3")
    mm.setGroupsAtLevel(0,[grp0,grp1,grp2,grp3])
    f=MEDCouplingFieldDouble(ON_GAUSS_NE) ; f.setMesh(m) ; f.setName("MyField") ; f.setTime(0.,0,0)
    arr2=DataArrayDouble(4*4*2) ; arr2.iota() ; arr2.rearrange(2) ; arr2.setInfoOnComponents(["aa","bbb"])
    f.setArray(arr2) ; arr2+=0.1 ; f.checkConsistencyLight()
    mm.write(fname,2)
    WriteFieldUsingAlreadyWrittenMesh(fname,f)
    return fname, arr2


@WriteInTmpDir
def test():
    fname,arr2 = GenerateCase()
    #
    reader=MEDReader(FileNames=[fname])
    ExpectedEntries=['TS0/Mesh/ComSup0/MyField@@][@@GSSNE','TS1/Mesh/ComSup0/Mesh@@][@@P0']
    assert(reader.GetProperty("FieldsTreeInfo")[::2]==ExpectedEntries)
    reader.FieldsStatus=['TS0/Mesh/ComSup0/MyField@@][@@GSSNE']
    ExtractGroup1 = ExtractGroup(Input=reader)
    #ExtractGroup1.UpdatePipelineInformation()
    ExtractGroup1.AllGroups=["GRP_grp1"]
    ELNOfieldToSurface1=ELNOfieldToSurface(Input=ExtractGroup1)
    ELNOfieldToPointGaussian1=ELNOfieldToPointGaussian(Input=ExtractGroup1)
    ELNOfieldToPointGaussian1.SelectSourceArray=['CELLS','ELNO@MyField']
    for elt in [ELNOfieldToSurface1,ELNOfieldToPointGaussian1]:
        elnoMesh=servermanager.Fetch(ELNOfieldToPointGaussian1,0)
        vtkArrToTest=elnoMesh.GetBlock(0).GetPointData().GetArray("MyField")
        assert(vtkArrToTest.GetNumberOfTuples()==8)
        assert(vtkArrToTest.GetNumberOfComponents()==2)
        assert(vtkArrToTest.GetComponentName(0)==arr2.getInfoOnComponent(0))
        assert(vtkArrToTest.GetComponentName(1)==arr2.getInfoOnComponent(1))
        vals=[vtkArrToTest.GetValue(i) for i in range(16)]
        assert(arr2[8:].isEqualWithoutConsideringStr(DataArrayDouble(vals,8,2),1e-12))
        pass
    #
    ExtractGroup1.AllGroups=["GRP_grp2"]
    for elt in [ELNOfieldToSurface1,ELNOfieldToPointGaussian1]:
        elnoMesh=servermanager.Fetch(ELNOfieldToSurface1)
        vtkArrToTest=elnoMesh.GetBlock(0).GetPointData().GetArray("MyField")
        assert(vtkArrToTest.GetNumberOfTuples()==8)
        assert(vtkArrToTest.GetNumberOfComponents()==2)
        assert(vtkArrToTest.GetComponentName(0)==arr2.getInfoOnComponent(0))
        assert(vtkArrToTest.GetComponentName(1)==arr2.getInfoOnComponent(1))
        vals=[vtkArrToTest.GetValue(i) for i in range(16)]
        assert(arr2[4:12].isEqualWithoutConsideringStr(DataArrayDouble(vals,8,2),1e-12))
        pass
    # important to check that if all the field is present that it is OK (check of the optimization)
    ExtractGroup1.AllGroups=["GRP_grp3"]
    for elt in [ELNOfieldToSurface1,ELNOfieldToPointGaussian1]:
        elnoMesh=servermanager.Fetch(ELNOfieldToSurface1)
        vtkArrToTest=elnoMesh.GetBlock(0).GetPointData().GetArray("MyField")
        assert(vtkArrToTest.GetNumberOfTuples()==16)
        assert(vtkArrToTest.GetNumberOfComponents()==2)
        assert(vtkArrToTest.GetComponentName(0)==arr2.getInfoOnComponent(0))
        assert(vtkArrToTest.GetComponentName(1)==arr2.getInfoOnComponent(1))
        vals=[vtkArrToTest.GetValue(i) for i in range(32)]
        assert(arr2.isEqualWithoutConsideringStr(DataArrayDouble(vals,16,2),1e-12))
        pass
    ELNOfieldToSurface1=ELNOfieldToSurface(Input=reader)
    ELNOfieldToPointGaussian1=ELNOfieldToPointGaussian(Input=reader)
    ELNOfieldToPointGaussian1.SelectSourceArray=['ELNO@MyField']
    for elt in [ELNOfieldToSurface1,ELNOfieldToPointGaussian1]:
        elnoMesh=servermanager.Fetch(ELNOfieldToSurface1)
        vtkArrToTest=elnoMesh.GetBlock(0).GetPointData().GetArray("MyField")
        assert(vtkArrToTest.GetNumberOfTuples()==16)
        assert(vtkArrToTest.GetNumberOfComponents()==2)
        assert(vtkArrToTest.GetComponentName(0)==arr2.getInfoOnComponent(0))
        assert(vtkArrToTest.GetComponentName(1)==arr2.getInfoOnComponent(1))
        vals=[vtkArrToTest.GetValue(i) for i in range(32)]
        assert(arr2.isEqualWithoutConsideringStr(DataArrayDouble(vals,16,2),1e-12))
        pass

if __name__ == "__main__":
  test()
  pass
