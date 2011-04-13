#include "vtkMedFieldOverEntity.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include "vtkMedUtilities.h"
#include "vtkMedMesh.h"
#include "vtkDataArray.h"
#include "vtkMedFieldOnProfile.h"
#include "vtkMedFieldStep.h"
#include "vtkMedField.h"

vtkCxxGetObjectVectorMacro(vtkMedFieldOverEntity, FieldOnProfile, vtkMedFieldOnProfile);
vtkCxxSetObjectVectorMacro(vtkMedFieldOverEntity, FieldOnProfile, vtkMedFieldOnProfile);

vtkCxxSetObjectMacro(vtkMedFieldOverEntity, ParentStep, vtkMedFieldStep);

vtkCxxRevisionMacro(vtkMedFieldOverEntity, "$Revision$")
vtkStandardNewMacro(vtkMedFieldOverEntity)

vtkMedFieldOverEntity::vtkMedFieldOverEntity()
{
	this->FieldOnProfile = new vtkObjectVector<vtkMedFieldOnProfile>();
	this->HasProfile = false;
	this->ParentStep = NULL;
}

vtkMedFieldOverEntity::~vtkMedFieldOverEntity()
{
	delete this->FieldOnProfile;
	this->SetParentStep(NULL);
}

void vtkMedFieldOverEntity::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
