//  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "VTKMEDCouplingUMeshClient.hxx"

#include "vtkPoints.h"
#include "vtkDoubleArray.h"
#include "vtkUnstructuredGrid.h"

#include <vector>
#include <string>

static const int ParaMEDMEM2VTKTypeTraducer[32]={1,3,21,5,9,7,22,-1,23,-1,-1,-1,-1,-1,10,14,13,-1,12,-1,24,-1,-1,27,-1,26,-1,-1,-1,-1,25,41};

void ParaMEDMEM2VTK::FillMEDCouplingUMeshInstanceFrom(SALOME_MED::MEDCouplingUMeshCorbaInterface_ptr meshPtr, vtkUnstructuredGrid *ret)
{
  meshPtr->Register();
  //
  SALOME_TYPES::ListOfLong *tinyI;
  SALOME_TYPES::ListOfString *tinyS;
  meshPtr->getTinyInfo(tinyI,tinyS);
  //
  int spaceDim=(*tinyI)[1];
  int nbOfNodes=(*tinyI)[2];
  int meshDim=(*tinyI)[3];
  int nbOfCells=(*tinyI)[4];
  int meshLength=(*tinyI)[5];
  std::string name((*tinyS)[0]);
  //std::vector<std::string> compoNames(spaceDim);
  //for(int i=0;i<spaceDim;i++)
  //  compoNames[i]=(*tinyS)[i+3];
  delete tinyI;
  delete tinyS;
  //
  ret->Initialize();
  ret->Allocate(nbOfCells);
  vtkPoints *points=vtkPoints::New();
  vtkDoubleArray *da=vtkDoubleArray::New();
  da->SetNumberOfComponents(spaceDim);
  da->SetNumberOfTuples(nbOfNodes);
  double *pts=da->GetPointer(0);
  //
  SALOME_TYPES::ListOfLong *a1Corba;
  SALOME_TYPES::ListOfDouble *a2Corba;
  meshPtr->getSerialisationData(a1Corba,a2Corba);
  int myLgth=a2Corba->length();
  for(int i=0;i<myLgth;i++)
    *pts++=(*a2Corba)[i];
  //
  int *tmp=new int[1000];
  for(int i=0;i<nbOfCells;i++)
    {
      int pos=(*a1Corba)[i];
      int pos1=(*a1Corba)[i+1];
      int nbOfNodeInCurCell=pos1-pos-1;
      int typeOfCell=(*a1Corba)[pos+nbOfCells+1];
      for(int j=0;j<nbOfNodeInCurCell;j++)
        tmp[j]=(*a1Corba)[pos+1+j+nbOfCells+1];
      ret->InsertNextCell(ParaMEDMEM2VTKTypeTraducer[typeOfCell],nbOfNodeInCurCell,tmp);
    }
  delete [] tmp;
  //
  delete a1Corba;
  delete a2Corba;
  //
  ret->SetPoints(points);
  points->Delete();
  points->SetData(da);
  da->Delete();
  //
  meshPtr->Destroy();
}
