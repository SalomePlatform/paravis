#include "vtkMedUnstructuredGrid.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"

#include "vtkMedUtilities.h"
#include "vtkMedEntityArray.h"
#include "vtkMedString.h"
#include "vtkMedMesh.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"

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

void vtkMedUnstructuredGrid::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_IVAR(os, indent, NumberOfPoints);
}
