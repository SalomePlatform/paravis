#include "vtkMedUnstructuredGrid.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"

#include "vtkMedUtilities.h"
#include "vtkMedEntityArray.h"
#include "vtkMedString.h"

vtkCxxGetObjectVectorMacro(vtkMedUnstructuredGrid, EntityArray, vtkMedEntityArray);
vtkCxxSetObjectVectorMacro(vtkMedUnstructuredGrid, EntityArray, vtkMedEntityArray);

vtkCxxSetObjectMacro(vtkMedUnstructuredGrid,Coordinates,vtkDataArray);

vtkCxxRevisionMacro(vtkMedUnstructuredGrid, "$Revision$")
vtkStandardNewMacro(vtkMedUnstructuredGrid)

vtkMedUnstructuredGrid::vtkMedUnstructuredGrid()
{
	this->Coordinates = NULL;
	this->NumberOfPoints = 0;
	this->CoordinateSystem = MED_CART;
  this->EntityArray = new vtkObjectVector<vtkMedEntityArray>();
}

vtkMedUnstructuredGrid::~vtkMedUnstructuredGrid()
{
	this->SetCoordinates(NULL);
	delete this->EntityArray;
}

int vtkMedUnstructuredGrid::IsCoordinatesLoaded()
{
	return this->Coordinates != NULL && this->Coordinates->GetNumberOfTuples()
			== this->NumberOfPoints;
}

vtkMedEntityArray* vtkMedUnstructuredGrid::GetEntityArray(med_entite_maillage type, med_geometrie_element geometry)
{
	for(int id = 0; id < this->EntityArray->size(); id++)
		{
		vtkMedEntityArray* array = this->EntityArray->at(id);
		if(array->GetType() == type && array->GetGeometry() == geometry)
			return array;
		}
	return NULL;
}

void  vtkMedUnstructuredGrid::InitializeCellGlobalIds()
{
  vtkIdType ncells = 0;

  med_entite_maillage type = MED_MAILLE;
  for(int geomIndex = 0; geomIndex < vtkMedUtilities::NumberOfCellGeometry; geomIndex++)
    {
    med_geometrie_element geom = vtkMedUtilities::CellGeometry[geomIndex];
    vtkMedEntityArray* array = this->GetEntityArray(type, geom);
    if(array == NULL)
      continue;

    array->SetInitialGlobalId(ncells + 1);

    ncells += array->GetNumberOfEntity();
    }

  type = MED_FACE;
  for(int geomIndex = 0; geomIndex < vtkMedUtilities::NumberOfFaceGeometry; geomIndex++)
    {
    med_geometrie_element geom = vtkMedUtilities::FaceGeometry[geomIndex];
    vtkMedEntityArray* array = this->GetEntityArray(type, geom);
    if(array == NULL)
      continue;

    array->SetInitialGlobalId(ncells + 1);

    ncells += array->GetNumberOfEntity();
    }

  type = MED_ARETE;
  for(int geomIndex = 0; geomIndex < vtkMedUtilities::NumberOfEdgeGeometry; geomIndex++)
    {
    med_geometrie_element geom = vtkMedUtilities::EdgeGeometry[geomIndex];
    vtkMedEntityArray* array = this->GetEntityArray(type, geom);
    if(array == NULL)
      continue;

    array->SetInitialGlobalId(ncells + 1);

    ncells += array->GetNumberOfEntity();
    }

  type = MED_NOEUD;
  for(int geomIndex = 0; geomIndex < vtkMedUtilities::NumberOfVertexGeometry; geomIndex++)
    {
    med_geometrie_element geom = vtkMedUtilities::VertexGeometry[geomIndex];
    vtkMedEntityArray* array = this->GetEntityArray(type, geom);
    if(array == NULL)
      continue;

    array->SetInitialGlobalId(ncells + 1);

    ncells += array->GetNumberOfEntity();
    }
}

void vtkMedUnstructuredGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, NumberOfPoints);
}
