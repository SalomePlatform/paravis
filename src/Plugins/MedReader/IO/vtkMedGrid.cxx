#include "vtkMedGrid.h"

#include "vtkObjectFactory.h"

#include "vtkMedUtilities.h"
#include "vtkMedMesh.h"
#include "vtkMedIntArray.h"
#include "vtkMedFamilyOnEntity.h"
#include "vtkMedFamily.h"
#include "vtkMedEntityArray.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"

#include <set>
using std::set;

vtkCxxGetObjectVectorMacro(vtkMedGrid, EntityArray, vtkMedEntityArray);
vtkCxxSetObjectVectorMacro(vtkMedGrid, EntityArray, vtkMedEntityArray);

vtkCxxSetObjectMacro(vtkMedGrid, PointGlobalIds, vtkMedIntArray);
vtkCxxSetObjectMacro(vtkMedGrid, ParentMesh, vtkMedMesh);
vtkCxxSetObjectMacro(vtkMedGrid, PreviousGrid, vtkMedGrid);

vtkCxxRevisionMacro(vtkMedGrid, "$Revision$")

vtkMedGrid::vtkMedGrid()
{
  this->ParentMesh = NULL;
  this->PointGlobalIds = NULL;
  this->PreviousGrid = NULL;

  this->CoordinateSystem = MED_CARTESIAN;
  this->EntityArray = new vtkObjectVector<vtkMedEntityArray>();
  this->UsePreviousCoordinates = false;
}

vtkMedGrid::~vtkMedGrid()
{
  this->SetPointGlobalIds(NULL);
  this->SetParentMesh(NULL);
  delete this->EntityArray;
}

int vtkMedGrid::IsPointGlobalIdsLoaded()
{
  return this->PointGlobalIds != NULL
      && this->PointGlobalIds->GetNumberOfTuples()
          == this->GetNumberOfPoints();
}

void  vtkMedGrid::ClearMedSupports()
{
  this->SetPointGlobalIds(NULL);
}

vtkMedEntityArray* vtkMedGrid::GetEntityArray(const vtkMedEntity& entity)
{
	for(int id = 0; id < this->EntityArray->size(); id++)
		{
		vtkMedEntityArray* array = this->EntityArray->at(id);
		if(array->GetEntity() == entity)
			return array;
		}
	return NULL;
}

void vtkMedGrid::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
