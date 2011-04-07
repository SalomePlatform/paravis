#include "vtkMedFamily.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"

#include "vtkMedGroup.h"
#include "vtkMedUtilities.h"
#include "vtkMedString.h"

vtkCxxGetObjectVectorMacro(vtkMedFamily, Group, vtkMedGroup);
vtkCxxSetObjectVectorMacro(vtkMedFamily, Group, vtkMedGroup);

vtkCxxRevisionMacro(vtkMedFamily, "$Revision$")
vtkStandardNewMacro(vtkMedFamily)

vtkMedFamily::vtkMedFamily()
{
	this->Id = 0;
	this->Name = vtkMedString::New();
	this->Name->SetSize(MED_NAME_SIZE);
	this->Name->SetString("UNDEFINED_FAMILY");
	this->Group = new vtkObjectVector<vtkMedGroup>();
	this->MedIterator = -1;
	this->PointOrCell = vtkMedUtilities::OnPoint;

	// by default, the family is part of the "NoGroup" fake group
	this->AllocateNumberOfGroup(1);
	vtkMedGroup* nogroup = this->GetGroup(0);
	nogroup->GetName()->SetString(vtkMedUtilities::NoGroupName);
}

vtkMedFamily::~vtkMedFamily()
{
	this->Name->Delete();
	delete this->Group;
}

void vtkMedFamily::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, Id);
	PRINT_IVAR(os, indent, MedIterator);
	PRINT_IVAR(os, indent, PointOrCell);
	PRINT_MED_STRING(os, indent, Name);
	PRINT_OBJECT_VECTOR(os, indent, Group);
}
