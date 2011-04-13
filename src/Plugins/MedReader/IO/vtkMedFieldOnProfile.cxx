#include "vtkMedFieldOnProfile.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"

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
	this->ProfileName = NULL;
	this->ProfileSize = 0;
	this->LocalizationName = NULL;
	this->NumberOfIntegrationPoint = 0;
	this->NumberOfValues = 0;
	this->Data = NULL;
	this->Profile = NULL;
	this->SetProfileName(MED_NO_PROFILE);
	this->SetLocalizationName(MED_NO_LOCALIZATION);
}

vtkMedFieldOnProfile::~vtkMedFieldOnProfile()
{
	this->SetProfileName(NULL);
	this->SetLocalizationName(NULL);
	this->SetParentFieldOverEntity(NULL);
	this->SetData(NULL);
}

int vtkMedFieldOnProfile::GetHasProfile()
{
	return strcmp(this->ProfileName, MED_NO_PROFILE) != 0;
}

int	vtkMedFieldOnProfile::IsLoaded()
{
	return this->Data!=NULL
			&& this->Data->GetNumberOfTuples() == this->NumberOfValues;
}

void	vtkMedFieldOnProfile::Load(med_storage_mode mode)
{
	this->ParentFieldOverEntity->GetParentStep()->GetParentField()
			->GetParentFile()->GetMedDriver()->LoadField(this, mode);
}

void vtkMedFieldOnProfile::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

  PRINT_IVAR(os, indent, MedIterator);
  PRINT_IVAR(os, indent, ProfileSize);
  PRINT_IVAR(os, indent, NumberOfIntegrationPoint);
  PRINT_IVAR(os, indent, NumberOfValues);
}
