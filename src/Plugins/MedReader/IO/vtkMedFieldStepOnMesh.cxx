#include "vtkMedFieldStepOnMesh.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkMedUtilities.h"
#include "vtkMedString.h"

#include "vtkDataArray.h"

vtkCxxSetObjectMacro(vtkMedFieldStepOnMesh, Data, vtkDataArray);

vtkCxxRevisionMacro(vtkMedFieldStepOnMesh, "$Revision$")
vtkStandardNewMacro(vtkMedFieldStepOnMesh)

vtkMedFieldStepOnMesh::vtkMedFieldStepOnMesh()
{
	this->NumberOfValues = 0;
	this->Data = NULL;
	this->NumberOfQuadraturePoint = 0;
	this->Local = 1;
	this->MeshName = vtkMedString::New();
	this->MeshName->SetSize(MED_TAILLE_NOM);
	this->ProfileName = vtkMedString::New();
	this->ProfileName->SetSize(MED_TAILLE_NOM);
	this->QuadratureDefinitionName = vtkMedString::New();
	this->QuadratureDefinitionName->SetSize(MED_TAILLE_NOM);
	this->MetaInfo = 0;
	this->HasProfile = 0;
}

vtkMedFieldStepOnMesh::~vtkMedFieldStepOnMesh()
{
	this->SetData(NULL);
	this->MeshName->Delete();
	this->ProfileName->Delete();
	this->QuadratureDefinitionName->Delete();
}

int	vtkMedFieldStepOnMesh::IsLoaded()
{
	return this->Data!=NULL && this->Data->GetNumberOfTuples() == this->NumberOfValues;
}

void vtkMedFieldStepOnMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, NumberOfValues);
	PRINT_MED_STRING(os, indent, MeshName);
	PRINT_MED_STRING(os, indent, ProfileName);
	PRINT_MED_STRING(os, indent, QuadratureDefinitionName);
	PRINT_IVAR(os, indent, NumberOfQuadraturePoint);
	PRINT_IVAR(os, indent, Local);
	PRINT_OBJECT(os, indent, Data);
}
