#include "vtkMedFieldOverEntity.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include "vtkMedFieldStep.h"
#include "vtkMedUtilities.h"

vtkCxxGetObjectVectorMacro(vtkMedFieldOverEntity, Step, vtkMedFieldStep);
vtkCxxSetObjectVectorMacro(vtkMedFieldOverEntity, Step, vtkMedFieldStep);

vtkCxxRevisionMacro(vtkMedFieldOverEntity, "$Revision$")
vtkStandardNewMacro(vtkMedFieldOverEntity)

vtkMedFieldOverEntity::vtkMedFieldOverEntity()
{
  this->MedType = MED_NOEUD;
  this->Geometry = MED_NONE;
  this->Step = new vtkObjectVector<vtkMedFieldStep>();
  this->IsELNO = 0;
}

vtkMedFieldOverEntity::~vtkMedFieldOverEntity()
{
	delete this->Step;
}

med_entite_maillage vtkMedFieldOverEntity::GetType()
{
  if(this->IsELNO == 1 && this->MedType == MED_NOEUD_MAILLE)
    return MED_MAILLE;
  return this->MedType;
}

void vtkMedFieldOverEntity::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_IVAR(os, indent, MedType);
  PRINT_IVAR(os, indent, Geometry);
  PRINT_IVAR(os, indent, IsELNO);
  PRINT_OBJECT_VECTOR(os, indent, Step);
}
