#include "vtkMedField.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"
#include "vtkMedFieldOverEntity.h"
#include "vtkMedFieldStepOnMesh.h"
#include "vtkMedFieldStep.h"
#include "vtkMedString.h"
#include "vtkMedUtilities.h"

#include <string>
using std::string;

vtkCxxGetObjectVectorMacro(vtkMedField, FieldOverEntity, vtkMedFieldOverEntity);
vtkCxxSetObjectVectorMacro(vtkMedField, FieldOverEntity, vtkMedFieldOverEntity);

vtkCxxGetObjectVectorMacro(vtkMedField, Unit, vtkMedString);
vtkCxxSetObjectVectorMacro(vtkMedField, Unit, vtkMedString);

vtkCxxGetObjectVectorMacro(vtkMedField, ComponentName, vtkMedString);
vtkCxxSetObjectVectorMacro(vtkMedField, ComponentName, vtkMedString);

vtkCxxRevisionMacro(vtkMedField, "$Revision$")
vtkStandardNewMacro(vtkMedField)

vtkMedField::vtkMedField()
{
	this->NumberOfComponent = -1;
	this->DataType = MED_FLOAT64;
	this->Name = vtkMedString::New();
	this->Name->SetSize(MED_TAILLE_NOM);
	this->FieldOverEntity = new vtkObjectVector<vtkMedFieldOverEntity> ();
	this->Unit = new vtkObjectVector<vtkMedString> ();
	this->ComponentName = new vtkObjectVector<vtkMedString> ();
	this->MedIndex = -1;
	this->Type = -1;
}

vtkMedField::~vtkMedField()
{
	this->Name->Delete();
	delete this->FieldOverEntity;
	delete this->Unit;
	delete this->ComponentName;
}

void vtkMedField::ComputeFieldSupportType()
{
	if (this->FieldOverEntity->size() == 0)
		{
		this->Type = -1;
		return;
		}

	string name = this->Name->GetString();
	if(name.find("ELGA") != string::npos || name.find("ELNO") != string::npos)
	  {
    this->Type = QuadratureField;
    return;
	  }

	for (int index = 0; index < this->FieldOverEntity->size(); index++)
		{
		vtkMedFieldOverEntity* fieldOverEntity = this->FieldOverEntity->at(index);
		med_entite_maillage type = fieldOverEntity->GetMedType();
		if (type == MED_NOEUD)
			{
			this->Type = PointField;
			return;
			}
		else
			{
			for (int stepIndex = 0; stepIndex < fieldOverEntity->GetNumberOfStep(); stepIndex++)
				{
				vtkMedFieldStep* step = fieldOverEntity->GetStep(stepIndex);
				for (int stepOnMeshId = 0; stepOnMeshId < step->GetNumberOfStepOnMesh(); stepOnMeshId++)
					{
					vtkMedFieldStepOnMesh* stepOnMesh = step->GetStepOnMesh(stepOnMeshId);
					if (stepOnMesh->GetNumberOfQuadraturePoint() > 1)
						{
						this->Type = QuadratureField;
						return;
						}
					}
				}
			}
		}
	this->Type = CellField;
}

void vtkMedField::SetNumberOfComponent(int ncomp)
{
	if (this->NumberOfComponent == ncomp)
		return;

	this->NumberOfComponent = ncomp;
	this->AllocateNumberOfUnit(this->NumberOfComponent);
	this->AllocateNumberOfComponentName(this->NumberOfComponent);
	for (int comp = 0; comp < this->NumberOfComponent; comp++)
		{
		this->Unit->at(comp)->SetSize(MED_TAILLE_PNOM);
		this->ComponentName->at(comp)->SetSize(MED_TAILLE_PNOM);
		}

	this->Modified();
}

vtkMedFieldOverEntity* vtkMedField::GetFieldOverEntity(	med_entite_maillage type,
																												med_geometrie_element geometry)
{
	for (int index = 0; index < this->FieldOverEntity->size(); index++)
		{
		vtkMedFieldOverEntity* fieldOverEntity = this->FieldOverEntity->at(index);
		if (fieldOverEntity->GetMedType() == type && fieldOverEntity->GetGeometry()
				== geometry)
			return fieldOverEntity;
		}
	return NULL;
}

void vtkMedField::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, MedIndex);
	PRINT_MED_STRING(os, indent, Name);
	PRINT_IVAR(os, indent, NumberOfComponent);
	PRINT_IVAR(os, indent, Type);
	PRINT_IVAR(os, indent, DataType);
	PRINT_OBJECT_VECTOR(os, indent, FieldOverEntity);
	PRINT_STRING_VECTOR(os, indent, Unit);
	PRINT_STRING_VECTOR(os, indent, ComponentName);
}
