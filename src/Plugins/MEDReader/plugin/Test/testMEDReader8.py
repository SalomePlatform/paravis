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
    """ This test is a non regression test. ExtractCellType then ExtractGroup.
    """
    fname="testMEDReader8.med"
    #########
    arr=DataArrayDouble([(0,0,0),(1,0,0),(2,0,0),(3,0,0),(0,1,0),(1,1,0),(2,1,0),(3,1,0),(0,2,0),(1,2,0),(2,2,0),(3,2,0),(0,3,0),(1,3,0),(2,3,0),(3,3,0)])
    m0=MEDCouplingUMesh("mesh",2) ; m0.setCoords(arr) ; m0.allocateCells()
    for elt in [[2,3,6],[3,7,6],[6,9,5],[6,10,9]]:
        m0.insertNextCell(NORM_TRI3,elt)
        pass
    for elt in [[0,4,5,1],[5,6,2,1],[4,8,9,5],[6,10,11,7],[8,12,13,9],[9,13,14,10],[10,14,15,11]]:
        m0.insertNextCell(NORM_QUAD4,elt)
        pass
    mm=MEDFileUMesh()
    mm.setMeshAtLevel(0,m0)
    grp0=DataArrayInt([0,1,2,5]) ; grp0.setName("grp0")
    mm.setGroupsAtLevel(0,[grp0])
    fmts=MEDFileFieldMultiTS()
    #
    fNode=MEDCouplingFieldDouble(ON_NODES) ; fNode.setName("fNode")
    fNode.setMesh(m0)
    fNode.setArray(DataArrayDouble([3,2,1,0,3.16,2.23,1.41,1,3.6,2.82,2.23,2,4.24,3.6,3.16,3]))
    fNode.getArray().setInfoOnComponent(0,"C0")
    fNode.setTime(0.5,1,1)
    f1ts=MEDFileField1TS() ; f1ts.setFieldNoProfileSBT(fNode) ; fmts.pushBackTimeStep(f1ts)
    #
    fNode.getArray().reverse()
    fNode.setTime(0.5,1,2)
    f1ts=MEDFileField1TS() ; f1ts.setFieldNoProfileSBT(fNode) ; fmts.pushBackTimeStep(f1ts)
    #
    fNode.getArray().reverse()
    fNode.setTime(0.5,2,1)
    f1ts=MEDFileField1TS() ; f1ts.setFieldNoProfileSBT(fNode) ; fmts.pushBackTimeStep(f1ts)
    #
    fNode.getArray().reverse()
    fNode.setTime(0.5,2,2)
    f1ts=MEDFileField1TS() ; f1ts.setFieldNoProfileSBT(fNode) ; fmts.pushBackTimeStep(f1ts)
    #
    mm.write(fname,2)
    fmts.write(fname,0)
    return fname


@WriteInTmpDir
def test(baseline_file):
    fname = GenerateCase()
    ################### MED write is done -> Go to MEDReader

    myMedReader=MEDReader(FileNames=[fname])
    myMedReader.FieldsStatus = ['TS0/mesh/ComSup0/fNode@@][@@P1']
    assert(list(myMedReader.TimestepValues)==[0.,1.,2.,3.])
    myMedReader.UpdatePipeline()

    extractCT=ExtractCellType(Input=myMedReader)
    extractCT.UpdatePipelineInformation()
    assert(list(extractCT.GetProperty("GeoTypesInfo"))==['Triangle', '0', 'Quad', '0'])
    extractCT.AllGeoTypes=['Triangle']

    extGrp=ExtractGroup(Input=extractCT)
    #extGrp.UpdatePipelineInformation()
    assert([x for x in list(extGrp.GetProperty("GroupsFlagsInfo")[::2]) if x[:4]=="GRP_"]==['GRP_grp0'])
    extGrp.AllGroups="GRP_grp0"

    if '-D' not in sys.argv:
        RenderView1 = GetRenderView()
        RenderView1.CameraFocalPoint = [1.5, 1.5, 0.0]
        RenderView1.CameraPosition = [1.5, 1.5, 10000.0]
        RenderView1.InteractionMode = '3D'
        RenderView1.CameraPosition = [1.5, 1.5, 8.196152422706632]
        RenderView1.CameraParallelScale = 2.1213203435596424
        RenderView1.CenterOfRotation = [1.5, 1.5, 0.0]

        DataRepresentation4 = Show()
        DataRepresentation4.EdgeColor = [0.0, 0.0, 0.5000076295109483]
        DataRepresentation4.SelectionPointFieldDataArrayName = 'fNode'
        DataRepresentation4.ScaleFactor = 0.3182729169726372

        a1_fGauss_PVLookupTable = GetLookupTableForArray( "fNode", 1, RGBPoints=[0.22, 0.23, 0.299, 0.754, 2.95, 0.706, 0.016, 0.15], VectorMode='Magnitude', NanColor=[0.25, 0.0, 0.0], ColorSpace='Diverging', ScalarRangeInitialized=1.0, AllowDuplicateScalars=1 )
        a1_fGauss_PiecewiseFunction = CreatePiecewiseFunction( Points=[0.0, 0.0, 0.5, 0.0, 1.0, 1.0, 0.5, 0.0] )
        DataRepresentation4.ColorArrayName = 'fNode'
        DataRepresentation4.LookupTable = a1_fGauss_PVLookupTable
        a1_fGauss_PVLookupTable.ScalarOpacityFunction = a1_fGauss_PiecewiseFunction

        RenderView1.ViewTime = 1.0 #### Important # red is in right bottom
        RenderView1.CacheKey = 1.0
        RenderView1.UseCache = 1
        RenderView1.ViewSize=[300,300]
        Render()

        # compare with baseline image
        
        import vtk.test.Testing
        from vtk.util.misc import vtkGetTempDir
        vtk.test.Testing.VTK_TEMP_DIR = vtk.util.misc.vtkGetTempDir()
        vtk.test.Testing.compareImage(GetActiveView().GetRenderWindow(), baseline_file,
                                                                    threshold=1)
        vtk.test.Testing.interact()

if __name__ == "__main__":
  outImgName="testMEDReader8.png"
  baseline_file = RetriveBaseLine(outImgName)
  test(baseline_file)
  pass
