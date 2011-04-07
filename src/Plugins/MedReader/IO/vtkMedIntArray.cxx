#include "vtkMedIntArray.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkMedIntArray, "$Revision$");
vtkStandardNewMacro(vtkMedIntArray);

//----------------------------------------------------------------------------
vtkMedIntArray::vtkMedIntArray(vtkIdType numComp): Superclass(numComp)
{
}

//----------------------------------------------------------------------------
vtkMedIntArray::~vtkMedIntArray()
{
}

//----------------------------------------------------------------------------
void vtkMedIntArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
