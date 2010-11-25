#include "vtkMedProfile.h"

#include "vtkObjectFactory.h"
#include "vtkMedIntArray.h"
#include "vtkMedString.h"
#include "vtkMedSetGet.h"
#include "vtkMedUtilities.h"

vtkCxxSetObjectMacro(vtkMedProfile,Ids,vtkMedIntArray)

vtkCxxRevisionMacro(vtkMedProfile, "$Revision$")
vtkStandardNewMacro(vtkMedProfile)

vtkMedProfile::vtkMedProfile()
{
	this->NumberOfElement = 0;
	this->Ids = NULL;
	this->Name = vtkMedString::New();
	this->Name->SetSize(MED_TAILLE_NOM);
	this->MedIndex = -1;
}

vtkMedProfile::~vtkMedProfile()
{
	this->Name->Delete();
	this->SetIds(NULL);
}

int vtkMedProfile::IsLoaded()
{
	return this->Ids != NULL && this->Ids->GetNumberOfComponents()
			== 1 && this->Ids->GetNumberOfTuples() == this->NumberOfElement
			&& this->NumberOfElement > 0;
}

void vtkMedProfile::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_MED_STRING(os, indent, Name);
	PRINT_IVAR(os, indent, NumberOfElement);
	PRINT_IVAR(os, indent, MedIndex);
	PRINT_OBJECT(os, indent, Ids);
}
