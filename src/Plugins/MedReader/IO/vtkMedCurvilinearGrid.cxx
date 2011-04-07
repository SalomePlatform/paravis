#include "vtkMedCurvilinearGrid.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"

vtkCxxSetObjectMacro(vtkMedCurvilinearGrid, Coordinates, vtkDataArray);

vtkCxxRevisionMacro(vtkMedCurvilinearGrid, "$Revision$")
vtkStandardNewMacro(vtkMedCurvilinearGrid)

vtkMedCurvilinearGrid::vtkMedCurvilinearGrid()
{
	this->Coordinates = NULL;
	this->NumberOfPoints = 0;
}

vtkMedCurvilinearGrid::~vtkMedCurvilinearGrid()
{
	this->SetCoordinates(NULL);
}

void vtkMedCurvilinearGrid::SetDimension(int dim)
{
	this->AxisSize.resize(dim);
}

int vtkMedCurvilinearGrid::GetDimension()
{
	return this->AxisSize.size();
}

void  vtkMedCurvilinearGrid::SetAxisSize(int axis, med_int size)
{
	if(axis < 0)
		return;

	if(axis >= this->AxisSize.size())
		this->AxisSize.resize(axis+1);

	this->AxisSize[axis] = size;
}

med_int vtkMedCurvilinearGrid::GetAxisSize(int axis)
{
	if(axis < 0 || axis >= this->AxisSize.size())
		return 0;

	return this->AxisSize[axis];
}

void  vtkMedCurvilinearGrid::LoadCoordinates()
{
	//TODO
}

void vtkMedCurvilinearGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
