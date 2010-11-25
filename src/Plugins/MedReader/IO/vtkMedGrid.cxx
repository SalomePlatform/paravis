#include "vtkMedGrid.h"

#include "vtkObjectFactory.h"

#include "vtkMedUtilities.h"
#include "vtkMedMesh.h"
#include "vtkMedString.h"

vtkCxxGetObjectVectorMacro(vtkMedGrid, Unit, vtkMedString);
vtkCxxSetObjectVectorMacro(vtkMedGrid, Unit, vtkMedString);

vtkCxxGetObjectVectorMacro(vtkMedGrid, ComponentName, vtkMedString);
vtkCxxSetObjectVectorMacro(vtkMedGrid, ComponentName, vtkMedString);

vtkCxxRevisionMacro(vtkMedGrid, "$Revision$")

vtkMedGrid::vtkMedGrid()
{
	this->Unit = new vtkObjectVector<vtkMedString> ();
	this->ComponentName = new vtkObjectVector<vtkMedString> ();
	this->Dimension = -1;
}

vtkMedGrid::~vtkMedGrid()
{
	delete this->Unit;
	delete this->ComponentName;
}

void vtkMedGrid::SetDimension(med_int dim)
{
	if (this->Dimension == dim)
		return;

	this->Dimension = dim;

	this->AllocateNumberOfUnit(dim);
	this->AllocateNumberOfComponentName(dim);
	for (int comp = 0; comp < dim; comp++)
		{
		this->Unit->at(comp)->SetSize(MED_TAILLE_PNOM);
		this->ComponentName->at(comp)->SetSize(MED_TAILLE_PNOM);
		}

	this->Modified();
}

void vtkMedGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, Dimension);
	PRINT_STRING_VECTOR(os, indent, Unit);
	PRINT_STRING_VECTOR(os, indent, ComponentName);
}
