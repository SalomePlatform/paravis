#include "vtkMedStandardGrid.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"

vtkCxxSetObjectMacro(vtkMedStandardGrid,Coordinates,vtkDataArray)
;

vtkCxxRevisionMacro(vtkMedStandardGrid, "$Revision$")
vtkStandardNewMacro(vtkMedStandardGrid)

vtkMedStandardGrid::vtkMedStandardGrid()
{
	this->Coordinates = NULL;
	this->CoordinateSystem = MED_CART;
	this->Size = NULL;
	this->NumberOfPoints = 0;
}

vtkMedStandardGrid::~vtkMedStandardGrid()
{
	this->SetCoordinates(NULL);
	if (this->Size != NULL)
		delete[] this->Size;
	this->Size = NULL;
}

void vtkMedStandardGrid::SetDimension(med_int dim)
{
	if (this->Size != NULL)
		delete[] this->Size;
	this->Size = NULL;
	if (dim > 0)
		{
		this->Size = new med_int[dim];
		}
	this->Superclass::SetDimension(dim);
}

void vtkMedStandardGrid::SetSize(med_int* size)
{
	if (this->Size == NULL || size == NULL)
		return;
	for (int dim = 0; dim < this->Dimension; dim++)
		{
		this->Size[dim] = size[dim];
		}
}

med_int* vtkMedStandardGrid::GetSize()
{
	return this->Size;
}

med_int vtkMedStandardGrid::GetSize(int dim)
{
	if (this->Size == NULL)
		return 0;
	return this->Size[dim];
}

void vtkMedStandardGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
