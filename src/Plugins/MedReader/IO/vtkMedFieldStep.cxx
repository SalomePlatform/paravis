#include "vtkMedFieldStep.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkMedUtilities.h"
#include "vtkMedFieldOverEntity.h"
#include "vtkMedField.h"

vtkCxxGetObjectVectorMacro(vtkMedFieldStep, FieldOverEntity, vtkMedFieldOverEntity);
vtkCxxSetObjectVectorMacro(vtkMedFieldStep, FieldOverEntity, vtkMedFieldOverEntity);

vtkCxxSetObjectMacro(vtkMedFieldStep, ParentField, vtkMedField);
vtkCxxSetObjectMacro(vtkMedFieldStep, PreviousStep, vtkMedFieldStep);

vtkCxxRevisionMacro(vtkMedFieldStep, "$Revision$")
vtkStandardNewMacro(vtkMedFieldStep)

vtkMedFieldStep::vtkMedFieldStep()
{
	this->FieldOverEntity = new vtkObjectVector<vtkMedFieldOverEntity>();
	this->PreviousStep = NULL;
	this->ParentField = NULL;
}

vtkMedFieldStep::~vtkMedFieldStep()
{
	delete this->FieldOverEntity;
	this->SetPreviousStep(NULL);
	this->SetParentField(NULL);
}

vtkMedFieldOverEntity* vtkMedFieldStep::GetFieldOverEntity(
		const vtkMedEntity& entity)
{
	for(int id=0; id < this->GetNumberOfFieldOverEntity(); id++)
		{
		vtkMedFieldOverEntity* fieldOverEntity = this->GetFieldOverEntity(id);
		if(fieldOverEntity->GetEntity() == entity)
			return fieldOverEntity;
		}
	return NULL;
}

void vtkMedFieldStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_OBJECT_VECTOR(os, indent, FieldOverEntity);
}
