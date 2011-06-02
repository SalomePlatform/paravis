// Copyright (C) 2010-2011  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#include "vtkMedUnstructuredGrid.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"
#include "vtkUnstructuredGrid.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkIdList.h"
#include "vtkCellType.h"

#include "vtkMedUtilities.h"
#include "vtkMedEntityArray.h"
#include "vtkMedMesh.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"
#include "vtkMedFamilyOnEntityOnProfile.h"
#include "vtkMedFamilyOnEntity.h"
#include "vtkMedProfile.h"
#include "vtkMedUtilities.h"

vtkCxxSetObjectMacro(vtkMedUnstructuredGrid,Coordinates,vtkDataArray);

vtkCxxRevisionMacro(vtkMedUnstructuredGrid, "$Revision$")
vtkStandardNewMacro(vtkMedUnstructuredGrid)

vtkMedUnstructuredGrid::vtkMedUnstructuredGrid()
{
	this->Coordinates = NULL;
	this->NumberOfPoints = 0;
}

vtkMedUnstructuredGrid::~vtkMedUnstructuredGrid()
{
	this->SetCoordinates(NULL);
}

int vtkMedUnstructuredGrid::IsCoordinatesLoaded()
{
  return this->Coordinates != NULL && this->Coordinates->GetNumberOfTuples()
     == this->NumberOfPoints;
}

void  vtkMedUnstructuredGrid::InitializeCellGlobalIds()
{
  vtkIdType ncells = 0;

  for(int entityIndex = 0; entityIndex < MED_N_ENTITY_TYPES; entityIndex++)
    {
    vtkMedEntity entity;
    entity.EntityType = MED_GET_ENTITY_TYPE[entityIndex+1];
    if(entity.EntityType == MED_NODE)
      continue;

    for(int geomIndex = 0; geomIndex < MED_N_CELL_FIXED_GEO; geomIndex++)
      {
      entity.GeometryType = MED_GET_CELL_GEOMETRY_TYPE[geomIndex+1];
      vtkMedEntityArray* array = this->GetEntityArray(entity);

      if(array == NULL)
        continue;

      array->SetInitialGlobalId(ncells + 1);

      ncells += array->GetNumberOfEntity();
      }
   }
}

void  vtkMedUnstructuredGrid::ClearMedSupports()
{
	this->Superclass::ClearMedSupports();
	for(int id = 0; id < this->EntityArray->size(); id++)
		{
		vtkMedEntityArray* array = this->EntityArray->at(id);
		array->Initialize();
		}
}

void  vtkMedUnstructuredGrid::LoadCoordinates()
{
	this->GetParentMesh()->GetParentFile()->GetMedDriver()->LoadCoordinates(this);
}

double* vtkMedUnstructuredGrid::GetCoordTuple(med_int index)
{
  return this->Coordinates->GetTuple(index);
}

vtkDataSet* vtkMedUnstructuredGrid::CreateVTKDataSet(
    vtkMedFamilyOnEntityOnProfile* foep)
{
  vtkMedFamilyOnEntity* foe = foep->GetFamilyOnEntity();
  vtkMedEntityArray* array = foe->GetEntityArray();
  if(foep->GetValid() == 0)
    {
    return NULL;
    }

  vtkUnstructuredGrid* vtkugrid = vtkUnstructuredGrid::New();

  // now we copy all the flagged nodes in the grid, shallow copy if possible
  bool shallowCopyPoints=true;

  if(this->GetParentMesh()->GetSpaceDimension()!=3)
    {
    shallowCopyPoints=false;
    }

  shallowCopyPoints = shallowCopyPoints && foep->CanShallowCopyPointField(NULL);

  foe->GetParentGrid()->LoadCoordinates();

  vtkIdType numberOfPoints;
  vtkPoints* points=vtkPoints::New(this->GetCoordinates()->GetDataType());
  vtkugrid->SetPoints(points);
  points->Delete();

  vtkIdTypeArray* pointGlobalIds=vtkIdTypeArray::New();
  pointGlobalIds->SetName("MED_POINT_ID");
  pointGlobalIds->SetNumberOfComponents(1);
  vtkugrid->GetPointData()->SetGlobalIds(pointGlobalIds);
  pointGlobalIds->Delete();

  vtkIdTypeArray* cellGlobalIds=vtkIdTypeArray::New();
  cellGlobalIds->SetName("MED_CELL_ID");
  cellGlobalIds->SetNumberOfComponents(1);
  vtkugrid->GetCellData()->SetGlobalIds(cellGlobalIds);
  cellGlobalIds->Delete();

  if (shallowCopyPoints)
    {
    vtkugrid->GetPoints()->SetDataType(this->GetCoordinates()->GetDataType());
    vtkugrid->GetPoints()->SetData(this->GetCoordinates());
    // add global ids
    numberOfPoints=this->GetNumberOfPoints();
    pointGlobalIds->SetNumberOfTuples(numberOfPoints);
    vtkIdType* ptr=pointGlobalIds->GetPointer(0);
    for(int pid=0; pid<numberOfPoints; pid++)
      ptr[pid]=pid+1;
    }
  else
    {

    vtkIdType currentIndex=0;
    for(vtkIdType index=0; index<this->GetNumberOfPoints(); index++)
      {
      if(!foep->KeepPoint(index))
        continue;

      double coord[3]={0.0, 0.0, 0.0};
      double * tuple=this->GetCoordinates()->GetTuple(index);
      for(int dim=0; dim<this->GetParentMesh()->GetSpaceDimension()&&dim<3; dim++)
        {
        coord[dim]=tuple[dim];
        }
      vtkugrid->GetPoints()->InsertPoint(currentIndex, coord);
      pointGlobalIds->InsertNextValue(index+1);
      currentIndex++;
      }
    vtkugrid->GetPoints()->Squeeze();
    pointGlobalIds->Squeeze();
    numberOfPoints=currentIndex;
    }

  int vtkType = vtkMedUtilities::GetVTKCellType(
      array->GetEntity().GeometryType);

  vtkSmartPointer<vtkIdList> pts = vtkSmartPointer<vtkIdList>::New();
  vtkIdType intialGlobalId = array->GetInitialGlobalId();
  vtkMedIntArray* pids = (foep->GetProfile()!=NULL?
                           foep->GetProfile()->GetIds() : NULL);
  vtkIdType maxId = (pids!=NULL?
                     pids->GetNumberOfTuples():
                     array->GetNumberOfEntity());

  for (vtkIdType pindex = 0; pindex<maxId && foep->GetValid(); pindex++)
    {
    vtkIdType realIndex = (pids!=NULL?
                           pids->GetValue(pindex)-1:
                           pindex);
    if (!foep->KeepCell(realIndex))
      continue;

    array->GetCellVertices(pindex, pts);

    cellGlobalIds->InsertNextValue(intialGlobalId+pindex);

    if (array->GetEntity().GeometryType==MED_POLYHEDRON)
      {
      if(vtkMedUtilities::FormatPolyhedronForVTK(foep, pindex, pts))
        {
        vtkugrid->InsertNextCell(VTK_POLYHEDRON, pts);
        }
      else
        {
        foep->SetValid(0);
        vtkugrid->Delete();
        return NULL;
        }
      }
    else
      {
      for(vtkIdType node=0; node<pts->GetNumberOfIds(); node++)
        {
        vtkIdType pid = pts->GetId(node);
        vtkIdType ptid=foep->GetVTKPointIndex(pid);
        if(ptid < 0 || ptid >= vtkugrid->GetNumberOfPoints())
          {
          vtkDebugMacro("Index error, this cell"  <<
                        " is not on this profile");
          vtkugrid->Delete();
          foep->SetValid(0);
          return NULL;
          }
        pts->SetId(vtkMedUtilities::MedToVTKIndex(vtkType, node), ptid);
        }
      vtkugrid->InsertNextCell(vtkType, pts);
      }
    }
  return vtkugrid;
}

void vtkMedUnstructuredGrid::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_IVAR(os, indent, NumberOfPoints);
}
