#include "vtkMedFieldStep.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkMedUtilities.h"
#include "vtkMedString.h"
#include "vtkMedMesh.h"
#include "vtkMedFieldStepOnMesh.h"

vtkCxxGetObjectVectorMacro(vtkMedFieldStep, StepOnMesh, vtkMedFieldStepOnMesh);
vtkCxxSetObjectVectorMacro(vtkMedFieldStep, StepOnMesh, vtkMedFieldStepOnMesh);

vtkCxxRevisionMacro(vtkMedFieldStep, "$Revision$")
vtkStandardNewMacro(vtkMedFieldStep)

vtkMedFieldStep::vtkMedFieldStep()
{
	this->Time = 0.0;
	this->TimeId = 0;
	this->Iteration = 0;
	this->TimeUnit = vtkMedString::New();
	this->TimeUnit->SetSize(MED_TAILLE_PNOM);
	this->StepOnMesh = new vtkObjectVector<vtkMedFieldStepOnMesh>();
}

vtkMedFieldStep::~vtkMedFieldStep()
{
	this->TimeUnit->Delete();
	delete this->StepOnMesh;
}

vtkMedFieldStepOnMesh* vtkMedFieldStep::GetStepOnMesh(vtkMedMesh* mesh)
{
	for(int id=0; id < this->GetNumberOfStepOnMesh(); id++)
		{
		vtkMedFieldStepOnMesh* stepOnMesh = this->GetStepOnMesh(id);
		if(strcmp(mesh->GetName()->GetString(), stepOnMesh->GetMeshName()->GetString()) == 0)
			{
			return stepOnMesh;
			}
		}
	return NULL;
}

void vtkMedFieldStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
	PRINT_IVAR(os, indent, Time);
	PRINT_IVAR(os, indent, TimeId);
	PRINT_IVAR(os, indent, Iteration);
	PRINT_MED_STRING(os, indent, TimeUnit);
	PRINT_OBJECT_VECTOR(os, indent, StepOnMesh);
}
