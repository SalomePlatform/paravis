#include "vtkMedFactory.h"

#include "vtkObjectFactory.h"

#include "vtkMedDriver30.h"

vtkCxxRevisionMacro(vtkMedFactory, "$Revision$")
vtkStandardNewMacro(vtkMedFactory)

vtkMedFactory::vtkMedFactory()
{
}

vtkMedFactory::~vtkMedFactory()
{
}

vtkMedDriver* vtkMedFactory::NewMedDriver(int major, int minor, int release)
{
  return vtkMedDriver30::New();
}

void vtkMedFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
