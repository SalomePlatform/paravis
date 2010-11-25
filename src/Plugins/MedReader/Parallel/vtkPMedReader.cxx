#include "vtkPMedReader.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkPMedReader, "$Revision$")
vtkStandardNewMacro(vtkPMedReader)

vtkPMedReader::vtkPMedReader()
{
}

vtkPMedReader::~vtkPMedReader()
{
}

int vtkPMedReader::RequestData(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
  return 1;
}

int vtkPMedReader::RequestInformation(vtkInformation *request,
    vtkInformationVector **inputVector,
    vtkInformationVector *outputVector)
{
  return 1;
}

void vtkPMedReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
