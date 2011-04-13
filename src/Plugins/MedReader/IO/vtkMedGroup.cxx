#include "vtkMedGroup.h"

#include "vtkObjectFactory.h"

#include "vtkMedUtilities.h"

vtkCxxRevisionMacro(vtkMedGroup, "$Revision$")
vtkStandardNewMacro(vtkMedGroup)

vtkMedGroup::vtkMedGroup()
{
	this->Name = NULL;
	this->SetName(vtkMedUtilities::NoGroupName);
}

vtkMedGroup::~vtkMedGroup()
{
	this->SetName(NULL);
}

void vtkMedGroup::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
