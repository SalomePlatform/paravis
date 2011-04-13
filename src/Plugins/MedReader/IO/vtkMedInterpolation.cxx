#include "vtkMedInterpolation.h"

#include "vtkObjectFactory.h"

#include "vtkMedUtilities.h"
#include "vtkMedFraction.h"

vtkCxxRevisionMacro(vtkMedInterpolation, "$Revision$")
vtkStandardNewMacro(vtkMedInterpolation)

vtkCxxGetObjectVectorMacro(vtkMedInterpolation, BasisFunction,
													 vtkMedFraction);
vtkCxxSetObjectVectorMacro(vtkMedInterpolation, BasisFunction,
													 vtkMedFraction);

vtkMedInterpolation::vtkMedInterpolation()
{
	this->GeometryType = MED_UNDEF_GEOTYPE;
	this->IsCellNode = 1;
	this->MaximumNumberOfCoefficient = 0;
	this->MaximumDegree = 0;
	this->NumberOfVariable = 0;
	this->Name = NULL;
	this->BasisFunction = new vtkObjectVector<vtkMedFraction>();
}

vtkMedInterpolation::~vtkMedInterpolation()
{
	delete this->BasisFunction;
	this->SetName(NULL);
}

void vtkMedInterpolation::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
