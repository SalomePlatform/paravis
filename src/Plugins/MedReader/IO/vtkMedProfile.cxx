#include "vtkMedProfile.h"

#include "vtkObjectFactory.h"
#include "vtkMedIntArray.h"
#include "vtkMedString.h"
#include "vtkMedSetGet.h"
#include "vtkMedUtilities.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"

vtkCxxSetObjectMacro(vtkMedProfile,Ids,vtkMedIntArray);
vtkCxxSetObjectMacro(vtkMedProfile,ParentFile,vtkMedFile);

vtkCxxRevisionMacro(vtkMedProfile, "$Revision$");
vtkStandardNewMacro(vtkMedProfile);

vtkMedProfile::vtkMedProfile()
{
	this->NumberOfElement = 0;
	this->Ids = NULL;
	this->Name = vtkMedString::New();
	this->Name->SetSize(MED_NAME_SIZE);
	this->MedIterator = -1;
	this->GeometryType = MED_NO_GEOTYPE;
	this->ParentFile = NULL;
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

void	vtkMedProfile::Load()
{
	this->ParentFile->GetMedDriver()->LoadProfile(this);
}

void vtkMedProfile::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_MED_STRING(os, indent, Name);
	PRINT_IVAR(os, indent, NumberOfElement);
	PRINT_IVAR(os, indent, MedIterator);
	PRINT_OBJECT(os, indent, Ids);
}
