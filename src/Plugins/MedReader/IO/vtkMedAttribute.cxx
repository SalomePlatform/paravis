#include "vtkMedAttribute.h"

#include "vtkObjectFactory.h"
#include "vtkMedUtilities.h"
#include "vtkMedString.h"
#include "vtkMedSetGet.h"

vtkCxxRevisionMacro(vtkMedAttribute, "$Revision$")
vtkStandardNewMacro(vtkMedAttribute)

vtkMedAttribute::vtkMedAttribute()
{
	this->Id = -1;
	this->Value = -1;
	this->Description = vtkMedString::New();
	this->Description->SetSize(MED_TAILLE_DESC);
}

vtkMedAttribute::~vtkMedAttribute()
{
	this->Description->Delete();
}

void vtkMedAttribute::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, Id);
	PRINT_IVAR(os, indent, Value);
	PRINT_MED_STRING(os, indent, Description);
}
