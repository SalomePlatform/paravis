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

#include "VTKMEDCouplingCMeshClient.hxx"

#include "vtkErrorCode.h"
#include "vtkDoubleArray.h"
#include "vtkRectilinearGrid.h"

#include <vector>
#include <string>

void ParaMEDMEM2VTK::FillMEDCouplingCMeshInstanceFrom(SALOME_MED::MEDCouplingCMeshCorbaInterface_ptr meshPtr, vtkRectilinearGrid *ret)
{
  meshPtr->Register();
  SALOME_TYPES::ListOfLong *tinyI;
  SALOME_TYPES::ListOfString *tinyS;
  meshPtr->getTinyInfo(tinyI,tinyS);
  int sizePerAxe[3];
  sizePerAxe[0]=(*tinyI)[0];
  sizePerAxe[1]=(*tinyI)[1];
  sizePerAxe[2]=(*tinyI)[2];
  ret->SetDimensions(sizePerAxe[0],sizePerAxe[1],sizePerAxe[2]);
  delete tinyI;
  delete tinyS;
  SALOME_TYPES::ListOfDouble *bigD;
  meshPtr->getSerialisationData(tinyI,bigD);
  delete tinyI;
  int offset=0;
  vtkDoubleArray *da=0;
  if(sizePerAxe[0]>0)
    {
      da=vtkDoubleArray::New();
      da->SetNumberOfTuples(sizePerAxe[0]);
      da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0);
      for(int i=0;i<sizePerAxe[0];i++)
        pt[i]=(*bigD)[i];
      ret->SetXCoordinates(da);
      da->Delete();
      offset+=sizePerAxe[0];
    }
  if(sizePerAxe[1]>0)
    {
      da=vtkDoubleArray::New();
      da->SetNumberOfTuples(sizePerAxe[1]);
      da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0);
      for(int i=0;i<sizePerAxe[1];i++)
        pt[i]=(*bigD)[offset+i];
      ret->SetYCoordinates(da);
      da->Delete();
      offset+=sizePerAxe[1];
    }
  if(sizePerAxe[2]>0)
    {
      da=vtkDoubleArray::New();
      da->SetNumberOfTuples(sizePerAxe[2]);
      da->SetNumberOfComponents(1);
      double *pt=da->GetPointer(0);
      for(int i=0;i<sizePerAxe[2];i++)
        pt[i]=(*bigD)[offset+i];
      ret->SetZCoordinates(da);
      da->Delete();
    }
  delete bigD;
  meshPtr->Destroy();
}
