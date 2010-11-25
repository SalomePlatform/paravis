#include "vtkMedFamily.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkStringArray.h"

#include "vtkMedGroup.h"
#include "vtkMedAttribute.h"
#include "vtkMedUtilities.h"
#include "vtkMedString.h"

vtkCxxGetObjectVectorMacro(vtkMedFamily, Group, vtkMedGroup);
vtkCxxSetObjectVectorMacro(vtkMedFamily, Group, vtkMedGroup);

vtkCxxGetObjectVectorMacro(vtkMedFamily, Attribute, vtkMedAttribute);
vtkCxxSetObjectVectorMacro(vtkMedFamily, Attribute, vtkMedAttribute);


vtkCxxRevisionMacro(vtkMedFamily, "$Revision$")
vtkStandardNewMacro(vtkMedFamily)

vtkMedFamily::vtkMedFamily()
{
	this->Id = 0;
	this->Name = vtkMedString::New();
	this->Name->SetSize(MED_TAILLE_NOM);
	this->Group = new vtkObjectVector<vtkMedGroup>();
	this->Attribute = new vtkObjectVector<vtkMedAttribute>();
	this->MedIndex = -1;
	this->PointOrCell = vtkMedUtilities::OnPoint;
}

vtkMedFamily::~vtkMedFamily()
{
	this->Name->Delete();
	delete this->Group;
	delete this->Attribute;
}

void	vtkMedFamily::ShallowCopy(vtkMedFamily* from)
{
	this->Id = from->Id;
	this->Name->SetString(from->GetName()->GetString());
	this->Group->resize(from->Group->size());
	for(int gid = 0; gid<this->Group->size(); gid++)
		{
		this->Group->at(gid) = from->Group->at(gid);
		}
	this->Attribute->resize(from->Attribute->size());
	for(int aid = 0; aid<this->Attribute->size(); aid++)
		{
		this->Attribute->at(aid) = from->Attribute->at(aid);
		}
	this->MedIndex = from->MedIndex;
}

void vtkMedFamily::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, Id);
	PRINT_IVAR(os, indent, MedIndex);
	PRINT_IVAR(os, indent, PointOrCell);
	PRINT_MED_STRING(os, indent, Name);
	PRINT_OBJECT_VECTOR(os, indent, Attribute);
	PRINT_OBJECT_VECTOR(os, indent, Group);
}
