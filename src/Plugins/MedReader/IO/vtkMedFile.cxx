#include "vtkMedFile.h"

#include "vtkObjectFactory.h"
#include "vtkDataArraySelection.h"
#include "vtkSmartPointer.h"

#include "vtkMedMesh.h"
#include "vtkMedField.h"
#include "vtkMedProfile.h"
#include "vtkMedQuadratureDefinition.h"
#include "vtkMedString.h"
#include "vtkMedUtilities.h"

vtkCxxGetObjectVectorMacro(vtkMedFile, Mesh, vtkMedMesh);
vtkCxxSetObjectVectorMacro(vtkMedFile, Mesh, vtkMedMesh);

vtkCxxGetObjectVectorMacro(vtkMedFile, Field, vtkMedField);
vtkCxxSetObjectVectorMacro(vtkMedFile, Field, vtkMedField);

vtkCxxGetObjectVectorMacro(vtkMedFile, Profile, vtkMedProfile);
vtkCxxSetObjectVectorMacro(vtkMedFile, Profile, vtkMedProfile);

vtkCxxGetObjectVectorMacro(vtkMedFile, QuadratureDefinition, vtkMedQuadratureDefinition);
vtkCxxSetObjectVectorMacro(vtkMedFile, QuadratureDefinition, vtkMedQuadratureDefinition);

vtkCxxRevisionMacro(vtkMedFile, "$Revision$")
vtkStandardNewMacro(vtkMedFile)

vtkMedFile::vtkMedFile()
{
	this->Description = vtkMedString::New();
	this->Description->SetSize(MED_TAILLE_DESC);
	this->Mesh = new vtkObjectVector<vtkMedMesh> ();
	this->Field = new vtkObjectVector<vtkMedField> ();
	this->Profile = new vtkObjectVector<vtkMedProfile> ();
	this->QuadratureDefinition
			= new vtkObjectVector<vtkMedQuadratureDefinition> ();
}

vtkMedFile::~vtkMedFile()
{
	this->Description->Delete();
	delete this->Mesh;
	delete this->Field;
	delete this->Profile;
	delete this->QuadratureDefinition;
}

vtkMedMesh* vtkMedFile::GetMesh(vtkMedString* str)
{
	for (int m = 0; m < this->Mesh->size(); m++)
		{
		vtkMedMesh* mesh = this->Mesh->at(m);
		if (strcmp(mesh->GetName()->GetString(), str->GetString()) == 0)
			{
			return mesh;
			}
		}
	return NULL;
}

vtkMedProfile* vtkMedFile::GetProfile(vtkMedString* str)
{
	for (int profId = 0; profId < this->Profile->size(); profId++)
		{
		vtkMedProfile* profile = this->Profile->at(profId);
		if (strcmp(profile->GetName()->GetString(), str->GetString()) == 0)
			{
			return profile;
			}
		}
	return NULL;

}

vtkMedQuadratureDefinition* vtkMedFile::GetQuadratureDefinition(vtkMedString* str)
{
	for (int quadId = 0; quadId < this->QuadratureDefinition->size(); quadId++)
		{
		vtkMedQuadratureDefinition* quaddef = this->QuadratureDefinition->at(quadId);
		if (strcmp(quaddef->GetName()->GetString(), str->GetString()) == 0)
			{
			return quaddef;
			}
		}
	return NULL;
}

void vtkMedFile::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);

	PRINT_MED_STRING(os, indent, Description);
	PRINT_OBJECT_VECTOR(os, indent, Mesh);
	PRINT_OBJECT_VECTOR(os, indent, Field);
	PRINT_OBJECT_VECTOR(os, indent, Profile);
	PRINT_OBJECT_VECTOR(os, indent, QuadratureDefinition);
}
