#include "vtkMedGroup.h"

#include "vtkObjectFactory.h"

#include "vtkMedUtilities.h"
#include "vtkMedString.h"

vtkCxxRevisionMacro(vtkMedGroup, "$Revision$")
vtkStandardNewMacro(vtkMedGroup)

vtkMedGroup::vtkMedGroup()
{
	this->Name = vtkMedString::New();
	this->Name->SetSize(MED_LNAME_SIZE);
	this->Name->SetString(vtkMedUtilities::NoGroupName);
}

vtkMedGroup::~vtkMedGroup()
{
	this->Name->Delete();
}

void vtkMedGroup::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
