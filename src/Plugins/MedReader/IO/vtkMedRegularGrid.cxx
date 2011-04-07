#include "vtkMedRegularGrid.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"

#include "vtkMedUtilities.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"
#include "vtkMedMesh.h"

vtkCxxGetObjectVectorMacro(vtkMedRegularGrid, AxisCoordinate, vtkDataArray);
vtkCxxSetAbstractObjectVectorMacro(vtkMedRegularGrid, AxisCoordinate, vtkDataArray);

vtkCxxRevisionMacro(vtkMedRegularGrid, "$Revision$")
vtkStandardNewMacro(vtkMedRegularGrid)

vtkMedRegularGrid::vtkMedRegularGrid()
{
	this->AxisCoordinate = new vtkObjectVector<vtkDataArray>();
}

vtkMedRegularGrid::~vtkMedRegularGrid()
{
	delete this->AxisCoordinate;
}

void vtkMedRegularGrid::SetDimension(med_int dim)
{
	if(dim < 0)
		dim = 0;
	this->AxisSize.resize(dim);
	this->SetNumberOfAxisCoordinate(dim);
}

int	vtkMedRegularGrid::GetDimension()
{
	return this->AxisSize.size();
}

void	vtkMedRegularGrid::SetAxisSize(int axis, med_int size)
{
	if(axis < 0)
		return;

	if(axis >= this->GetDimension())
		{
		this->SetDimension(axis+1);
		}

	this->AxisSize[axis] = size;
}

med_int vtkMedRegularGrid::GetAxisSize(int dim)
{
	if(dim < 0 || dim >= this->AxisSize.size())
		return 0;
	return this->AxisSize[dim];
}

med_int vtkMedRegularGrid::GetNumberOfPoints()
{
	med_int npts = 1;
	for(int dim = 0; dim < this->AxisSize.size(); dim++)
		{
		npts *= this->AxisSize[dim];
		}
	return npts;
}

void	vtkMedRegularGrid::LoadCoordinates()
{
	this->GetParentMesh()->GetParentFile()->GetMedDriver()->LoadCoordinates(this);
}

void vtkMedRegularGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
