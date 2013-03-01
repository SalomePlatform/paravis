// Copyright (C) 2010-2011  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "vtkMedFieldStep.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkMedUtilities.h"
#include "vtkMedFieldOverEntity.h"
#include "vtkMedField.h"
#include "vtkMedFile.h"
#include "vtkMedDriver.h"

vtkCxxGetObjectVectorMacro(vtkMedFieldStep, FieldOverEntity, vtkMedFieldOverEntity);
vtkCxxSetObjectVectorMacro(vtkMedFieldStep, FieldOverEntity, vtkMedFieldOverEntity);

vtkCxxSetObjectMacro(vtkMedFieldStep, ParentField, vtkMedField);
vtkCxxSetObjectMacro(vtkMedFieldStep, PreviousStep, vtkMedFieldStep);

// vtkCxxRevisionMacro(vtkMedFieldStep, "$Revision$")
vtkStandardNewMacro(vtkMedFieldStep)

vtkMedFieldStep::vtkMedFieldStep()
{
  this->FieldOverEntity = new vtkObjectVector<vtkMedFieldOverEntity>();
  this->PreviousStep = NULL;
  this->ParentField = NULL;
  this->MedIterator = -1;
  this->EntityInfoLoaded = 0;
}

vtkMedFieldStep::~vtkMedFieldStep()
{
  delete this->FieldOverEntity;
  this->SetPreviousStep(NULL);
  this->SetParentField(NULL);
}

vtkMedFieldOverEntity* vtkMedFieldStep::GetFieldOverEntity(
    const vtkMedEntity& entity)
{
  for(int id=0; id < this->GetNumberOfFieldOverEntity(); id++)
    {
    vtkMedFieldOverEntity* fieldOverEntity = this->GetFieldOverEntity(id);
    if(fieldOverEntity->GetEntity() == entity)
      return fieldOverEntity;
    }
  return NULL;
}

void  vtkMedFieldStep::LoadInformation()
{
  this->GetParentField()->GetParentFile()->GetMedDriver()->
      ReadFieldStepInformation(this, true);
}

void vtkMedFieldStep::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_OBJECT_VECTOR(os, indent, FieldOverEntity);
}
