#include "vtkMedEntityArray.h"

#include "vtkObjectFactory.h"
#include "vtkMedIntArray.h"

#include "vtkMedUtilities.h"
#include "vtkMedFamily.h"
#include "vtkMedFamilyOnEntity.h"
#include "vtkMedMesh.h"

#include <set>
using std::set;

vtkCxxSetObjectVectorMacro(vtkMedEntityArray, FamilyOnEntity, vtkMedFamilyOnEntity)
vtkCxxGetObjectVectorMacro(vtkMedEntityArray, FamilyOnEntity, vtkMedFamilyOnEntity)
vtkCxxSetObjectMacro(vtkMedEntityArray,FamilyIds,vtkMedIntArray)
vtkCxxSetObjectMacro(vtkMedEntityArray,GlobalIds,vtkMedIntArray)
vtkCxxSetObjectMacro(vtkMedEntityArray,ConnectivityArray,vtkMedIntArray)
vtkCxxSetObjectMacro(vtkMedEntityArray,Index0,vtkMedIntArray)
vtkCxxSetObjectMacro(vtkMedEntityArray,Index1,vtkMedIntArray)

vtkCxxRevisionMacro(vtkMedEntityArray, "$Revision$")
vtkStandardNewMacro(vtkMedEntityArray)

vtkMedEntityArray::vtkMedEntityArray()
{
  this->NumberOfEntity = 0;
  this->Type = MED_NOEUD;
  this->Geometry = MED_NONE;
  this->Connectivity = MED_NOD;
  this->FamilyIds = NULL;
  this->GlobalIds = NULL;
  this->ConnectivityArray = NULL;
  this->Index0 = NULL;
  this->Index1 = NULL;
  this->InitialGlobalId = 0;
  this->FamilyOnEntity = new vtkObjectVector<vtkMedFamilyOnEntity> ();
}

vtkMedEntityArray::~vtkMedEntityArray()
{
  this->SetFamilyIds(NULL);
  this->SetGlobalIds(NULL);
  this->SetConnectivityArray(NULL);
  this->SetIndex0(NULL);
  this->SetIndex1(NULL);
  delete this->FamilyOnEntity;
}

void vtkMedEntityArray::ComputeFamilies(vtkMedMesh* mesh)
{
  this->FamilyOnEntity->clear();

  if(this->FamilyIds == NULL)
    {
    vtkMedFamilyOnEntity* foe = vtkMedFamilyOnEntity::New();
    foe->SetMesh(mesh);
    this->AppendFamilyOnEntity(foe);
    foe->Delete();
    foe->SetFamily(mesh->GetOrCreateCellFamilyById(0));
    foe->SetEntityArray(this);
    return;
    }

  set<med_int> idset;
  for (vtkIdType index = 0; index < this->FamilyIds->GetNumberOfTuples(); index++)
    {
    med_int id = this->FamilyIds->GetValue(index);
    idset.insert(id);
    }
  for (set<med_int>::iterator it = idset.begin(); it != idset.end(); it++)
    {
    vtkMedFamilyOnEntity* foe = vtkMedFamilyOnEntity::New();
    foe->SetMesh(mesh);
    this->AppendFamilyOnEntity(foe);
    foe->Delete();
    foe->SetFamily(mesh->GetOrCreateCellFamilyById(*it));
    foe->SetEntityArray(this);
    }
}

int vtkMedEntityArray::HasFamily(vtkMedFamily* family)
{
  for (int i = 0; i < this->FamilyOnEntity->size(); i++)
    {
    vtkMedFamilyOnEntity* foe = this->FamilyOnEntity->at(i);
    if(foe->GetFamily() == family)
      return 1;
    }
  return 0;
}

int vtkMedEntityArray::IsConnectivityLoaded()
{
    if(this->ConnectivityArray == NULL)
      return 0;

  if(this->Connectivity == MED_NOD)
    {
    vtkIdType connSize = this->NumberOfEntity
        * vtkMedUtilities::GetNumberOfPoint(this->Geometry);

    return connSize == this->ConnectivityArray->GetNumberOfTuples();
    }
  else
    {
    vtkIdType connSize = this->NumberOfEntity
        * vtkMedUtilities::GetNumberOfSubEntity(this->Geometry);

    return connSize == this->ConnectivityArray->GetNumberOfTuples();
    }
}

int vtkMedEntityArray::IsFamilyIdsLoaded()
{
  return this->FamilyIds != NULL && this->FamilyIds->GetNumberOfTuples()
      == this->NumberOfEntity;
}

int vtkMedEntityArray::IsGlobalIdsLoaded()
{
  return this->GlobalIds != NULL && this->GlobalIds->GetNumberOfTuples()
      == this->NumberOfEntity;
}

void vtkMedEntityArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_IVAR(os, indent, NumberOfEntity)
  PRINT_IVAR(os, indent, Type)
  PRINT_IVAR(os, indent, Geometry)
  PRINT_IVAR(os, indent, Connectivity)
  PRINT_IVAR(os, indent, InitialGlobalId)
}
