// Copyright (C) 2010-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony Geay

#include "vtkGenerateVectors.h"
#include "vtkAOSDataArrayTemplate.h"
#include "vtkDoubleArray.h"
#include "vtkInformation.h"
#include "vtkUnstructuredGrid.h"
#include "vtkQuadratureSchemeDefinition.h"
#include "vtkInformationQuadratureSchemeDefinitionVectorKey.h"
#include "MEDUtilities.hxx"
#include "vtkFieldData.h"

#include <sstream>

const char vtkGenerateVectors::VECTOR_SUFFIX[]="_Vector";

std::string vtkGenerateVectors::SuffixFieldName(const std::string& name)
{
  std::ostringstream oss; oss << name << VECTOR_SUFFIX;
  return oss.str();
}

/*!
 * This method forces MeshMTime modification. To do so, points are declared as modified.
 */
void vtkGenerateVectors::ChangeMeshTimeToUpdateCache(vtkDataSet *dataset)
{
  vtkUnstructuredGrid *ug(vtkUnstructuredGrid::SafeDownCast(dataset));
  if(!ug)
    return ;
  ug->GetPoints()->Modified();
}

void vtkGenerateVectors::Operate(vtkFieldData *fd)
{
  if(!fd)
    return ;
  const int nbOfArrs(fd->GetNumberOfArrays());
  std::vector<vtkDoubleArray *> daToAppend;
  for(int i=0;i<nbOfArrs;i++)
    {
      vtkDataArray *arr(fd->GetArray(i));
      if(!arr)
        continue;
      vtkDoubleArray *arrc(vtkDoubleArray::SafeDownCast(arr));
      if(!arrc)
        continue;
      int nbOfCompo(arrc->GetNumberOfComponents());
      if(nbOfCompo<=1 || nbOfCompo==3)
        continue;
      if(nbOfCompo==2)
        daToAppend.push_back(Operate2Compo(arrc));
      else
        daToAppend.push_back(OperateMoreThan3Compo(arrc));
    }
  for(std::vector<vtkDoubleArray *>::const_iterator it=daToAppend.begin();it!=daToAppend.end();it++)
    {
      vtkDoubleArray *elt(*it);
      if(!elt)
	continue;
      fd->AddArray(elt);
      elt->Delete();
    }
}

vtkDoubleArray *vtkGenerateVectors::Operate2Compo(vtkDoubleArray *oldArr)
{
  const int VTK_DATA_ARRAY_FREE=vtkAOSDataArrayTemplate<double>::VTK_DATA_ARRAY_FREE;
  vtkDoubleArray *ret(vtkDoubleArray::New());
  vtkIdType nbOfTuples(oldArr->GetNumberOfTuples());
  const double *inPt(oldArr->GetPointer(0));
  double *pt((double *)malloc(nbOfTuples*3*sizeof(double)));
  for(vtkIdType i=0;i<nbOfTuples;i++)
    {
      pt[3*i+0]=inPt[2*i+0];
      pt[3*i+1]=inPt[2*i+1];
      pt[3*i+2]=0.;
    }
  ret->SetNumberOfComponents(3);
  std::string newName(SuffixFieldName(oldArr->GetName()));
  ret->SetName(newName.c_str());
  ret->SetComponentName(0,oldArr->GetComponentName(0));
  ret->SetComponentName(1,oldArr->GetComponentName(1));
  ret->SetArray(pt,3*nbOfTuples,0,VTK_DATA_ARRAY_FREE);
  UpdateInformationOfArray(oldArr,ret);
  return ret;
}

vtkDoubleArray *vtkGenerateVectors::OperateMoreThan3Compo(vtkDoubleArray *oldArr)
{
  const int VTK_DATA_ARRAY_FREE=vtkAOSDataArrayTemplate<double>::VTK_DATA_ARRAY_FREE;
  vtkDoubleArray *ret(vtkDoubleArray::New());
  int nbOfCompo(oldArr->GetNumberOfComponents());
  vtkIdType nbOfTuples(oldArr->GetNumberOfTuples());
  const double *inPt(oldArr->GetPointer(0));
  double *pt((double *)malloc(nbOfTuples*3*sizeof(double)));
  for(vtkIdType i=0;i<nbOfTuples;i++)
    {
      pt[3*i+0]=inPt[nbOfCompo*i+0];
      pt[3*i+1]=inPt[nbOfCompo*i+1];
      pt[3*i+2]=inPt[nbOfCompo*i+2];
    }
  ret->SetNumberOfComponents(3);
  std::string newName(SuffixFieldName(oldArr->GetName()));
  ret->SetName(newName.c_str());
  ret->SetComponentName(0,oldArr->GetComponentName(0));
  ret->SetComponentName(1,oldArr->GetComponentName(1));
  ret->SetComponentName(2,oldArr->GetComponentName(2));
  ret->SetArray(pt,3*nbOfTuples,0,VTK_DATA_ARRAY_FREE);
  UpdateInformationOfArray(oldArr,ret);
  return ret;
}

void vtkGenerateVectors::UpdateInformationOfArray(vtkDoubleArray *oldArr, vtkDoubleArray *arr)
{
  if(oldArr->GetInformation()->Has(vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME()))
    {
      arr->GetInformation()->Set(vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME(),oldArr->GetInformation()->Get((vtkQuadratureSchemeDefinition::QUADRATURE_OFFSET_ARRAY_NAME())));
    }
  if(oldArr->GetInformation()->Get(MEDUtilities::ELGA()))
    arr->GetInformation()->Set(MEDUtilities::ELGA(),1);
  vtkInformationQuadratureSchemeDefinitionVectorKey *key(vtkQuadratureSchemeDefinition::DICTIONARY());
  if(key->Has(oldArr->GetInformation()))
    {
      int dictSize(key->Size(oldArr->GetInformation()));
      vtkQuadratureSchemeDefinition **dict = new vtkQuadratureSchemeDefinition *[dictSize];
      key->GetRange(oldArr->GetInformation(),dict,0,0,dictSize);
      key->SetRange(arr->GetInformation(),dict,0,0,dictSize);
      delete [] dict;
    }
  if(oldArr->GetInformation()->Get(MEDUtilities::ELNO()))
    arr->GetInformation()->Set(MEDUtilities::ELNO(),1);
}
