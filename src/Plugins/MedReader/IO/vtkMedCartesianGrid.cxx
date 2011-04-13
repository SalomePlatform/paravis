#include "vtkMedCartesianGrid.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMedCartesianGrid, "$Revision$")
vtkStandardNewMacro(vtkMedCartesianGrid)

vtkMedCartesianGrid::vtkMedCartesianGrid()
{
}

vtkMedCartesianGrid::~vtkMedCartesianGrid()
{
}

void vtkMedCartesianGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
