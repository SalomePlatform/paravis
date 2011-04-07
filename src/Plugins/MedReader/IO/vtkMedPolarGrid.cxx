#include "vtkMedPolarGrid.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMedPolarGrid, "$Revision$")
vtkStandardNewMacro(vtkMedPolarGrid)

vtkMedPolarGrid::vtkMedPolarGrid()
{
}

vtkMedPolarGrid::~vtkMedPolarGrid()
{
}

void  vtkMedPolarGrid::LoadCoordinates()
{
  //TODO
}

void vtkMedPolarGrid::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
