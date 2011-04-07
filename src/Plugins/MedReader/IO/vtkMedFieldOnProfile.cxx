#include "vtkMedFieldOnProfile.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"

#include "vtkMedString.h"
#include "vtkMedFieldOverEntity.h"
#include "vtkMedFieldStep.h"
#include "vtkMedField.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"
#include "vtkMedProfile.h"

vtkCxxSetObjectMacro(vtkMedFieldOnProfile, Data, vtkDataArray);
vtkCxxSetObjectMacro(vtkMedFieldOnProfile, Profile, vtkMedProfile);
vtkCxxSetObjectMacro(vtkMedFieldOnProfile, ParentFieldOverEntity, vtkMedFieldOverEntity);

vtkCxxRevisionMacro(vtkMedFieldOnProfile, "$Revision$")
vtkStandardNewMacro(vtkMedFieldOnProfile)

vtkMedFieldOnProfile::vtkMedFieldOnProfile()
{
	this->MedIterator = -1;
	this->ParentFieldOverEntity = NULL;
	this->ProfileName = vtkMedString::New();
	this->ProfileName->SetSize(MED_NAME_SIZE);
	this->ProfileName->SetString(MED_NO_PROFILE);
	this->ProfileSize = 0;
	this->LocalizationName = vtkMedString::New();
	this->LocalizationName->SetSize(MED_NAME_SIZE);
	this->LocalizationName->SetString(MED_NO_LOCALIZATION);
	this->NumberOfIntegrationPoint = 0;
	this->NumberOfValues = 0;
	this->Data = NULL;
	this->Profile = NULL;
}

vtkMedFieldOnProfile::~vtkMedFieldOnProfile()
{
	this->ProfileName->Delete();
	this->LocalizationName->Delete();
	this->SetParentFieldOverEntity(NULL);
	this->SetData(NULL);
}

int vtkMedFieldOnProfile::GetHasProfile()
{
	return strcmp(this->ProfileName->GetString(), MED_NO_PROFILE) != 0;
}

int	vtkMedFieldOnProfile::IsLoaded()
{
	return this->Data!=NULL
			&& this->Data->GetNumberOfTuples() == this->NumberOfValues;
}

void	vtkMedFieldOnProfile::Load()
{
	this->ParentFieldOverEntity->GetParentStep()->GetParentField()
			->GetParentFile()->GetMedDriver()->LoadField(this);
}

void vtkMedFieldOnProfile::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

  PRINT_IVAR(os, indent, MedIterator);
  PRINT_MED_STRING(os, indent, ProfileName);
  PRINT_IVAR(os, indent, ProfileSize);
  PRINT_MED_STRING(os, indent, LocalizationName);
  PRINT_IVAR(os, indent, NumberOfIntegrationPoint);
  PRINT_IVAR(os, indent, NumberOfValues);
}
