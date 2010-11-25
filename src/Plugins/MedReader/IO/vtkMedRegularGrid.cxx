#include "vtkMedRegularGrid.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"

#include "vtkMedUtilities.h"

vtkCxxGetObjectVectorMacro(vtkMedRegularGrid, Coordinates, vtkDataArray);
vtkCxxSetAbstractObjectVectorMacro(vtkMedRegularGrid, Coordinates, vtkDataArray);

vtkCxxRevisionMacro(vtkMedRegularGrid, "$Revision$")
vtkStandardNewMacro(vtkMedRegularGrid)

vtkMedRegularGrid::vtkMedRegularGrid()
{
	this->Coordinates = new vtkObjectVector<vtkDataArray>();
	this->Size = NULL;
}

vtkMedRegularGrid::~vtkMedRegularGrid()
{
	delete this->Coordinates;
	if (this->Size)
		delete[] this->Size;
	this->Size = NULL;
}

void vtkMedRegularGrid::SetDimension(med_int dim)
{
	if (this->Size)
		delete[] this->Size;
	this->Size = NULL;
	if (dim > 0)
		{
		this->Size = new med_int[dim];
		}
	this->SetNumberOfCoordinates(dim);
	this->Superclass::SetDimension(dim);
}

void vtkMedRegularGrid::SetSize(med_int* size)
{
	if (this->Size == NULL || size == NULL)
		return;
	for (int dim = 0; dim < this->Dimension; dim++)
		{
		this->Size[dim] = size[dim];
		}
}

med_int* vtkMedRegularGrid::GetSize()
{
	return this->Size;
}

med_int vtkMedRegularGrid::GetSize(int dim)
{
	if (this->Size == NULL)
		return 0;
	return this->Size[dim];
}

med_int vtkMedRegularGrid::GetNumberOfPoints()
{
	med_int npts = 1;
	for(int dim = 0; dim < this->GetDimension(); dim++)
		{
		npts *= this->Size[dim];
		}
	return npts;
}

void vtkMedRegularGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
