#include "vtkMedFactory.h"

#include "vtkObjectFactory.h"

#include "vtkMedDriver23.h"

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
  if(major == 2 && minor > 1)
    return vtkMedDriver23::New();
  vtkErrorMacro("No known driver for file of version " << major << "." << minor << "." << release << ". You should use the medimport utility to import your med file to a newer version.");
  return NULL;
}

void vtkMedFactory::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
