#include "vtkMedFamilyOnEntity.h"

#include "vtkObjectFactory.h"
#include "vtkMedUtilities.h"
#include "vtkMedFamily.h"
#include "vtkMedEntityArray.h"
#include "vtkMedMesh.h"
#include "vtkMedGrid.h"
#include "vtkMedFamilyOnEntityOnProfile.h"

vtkCxxSetObjectMacro(vtkMedFamilyOnEntity, Family, vtkMedFamily);
vtkCxxSetObjectMacro(vtkMedFamilyOnEntity, EntityArray, vtkMedEntityArray);

vtkCxxSetObjectMacro(vtkMedFamilyOnEntity, ParentGrid,vtkMedGrid);

vtkCxxRevisionMacro(vtkMedFamilyOnEntity, "$Revision$");
vtkStandardNewMacro(vtkMedFamilyOnEntity)

vtkMedFamilyOnEntity::vtkMedFamilyOnEntity()
{
  this->Family = NULL;
  this->EntityArray = NULL;
  this->ParentGrid = NULL;
}

vtkMedFamilyOnEntity::~vtkMedFamilyOnEntity()
{
  this->SetFamily(NULL);
  this->SetEntityArray(NULL);
  this->SetParentGrid(NULL);
}

vtkMedEntity vtkMedFamilyOnEntity::GetEntity()
{
  if(this->EntityArray != NULL)
    {
    return this->EntityArray->GetEntity();
    }

  return vtkMedEntity(MED_NODE, MED_POINT1);
}

int vtkMedFamilyOnEntity::GetPointOrCell()
{
  if(this->GetEntity().EntityType == MED_NODE)
    return vtkMedUtilities::OnPoint;
  return vtkMedUtilities::OnCell;
}

int vtkMedFamilyOnEntity::GetVertexOnly()
{
  if(this->GetPointOrCell() == vtkMedUtilities::OnPoint ||
     this->EntityArray == NULL)
    return true;

  vtkMedEntity entity = this->EntityArray->GetEntity();
  if(entity.EntityType == MED_POINT1 || entity.GeometryType == MED_NONE)
    return true;

  return false;
}

void  vtkMedFamilyOnEntity::AddFamilyOnEntityOnProfile(
    vtkMedFamilyOnEntityOnProfile* foep)
{
  this->FamilyOnEntityOnProfile[foep->GetProfile()] = foep;
}

int vtkMedFamilyOnEntity::GetNumberOfFamilyOnEntityOnProfile()
{
  return this->FamilyOnEntityOnProfile.size();
}

vtkMedFamilyOnEntityOnProfile* vtkMedFamilyOnEntity::
    GetFamilyOnEntityOnProfile(vtkMedProfile* profile)
{
  if(this->FamilyOnEntityOnProfile.find(profile)
    != this->FamilyOnEntityOnProfile.end())
    return this->FamilyOnEntityOnProfile[profile];

  return NULL;
}

vtkMedFamilyOnEntityOnProfile* vtkMedFamilyOnEntity::
    GetFamilyOnEntityOnProfile(int index)
{
  if(index < 0 || index >= this->FamilyOnEntityOnProfile.size())
    return NULL;

  std::map<vtkMedProfile*,
  vtkSmartPointer<vtkMedFamilyOnEntityOnProfile> >::iterator it =
  this->FamilyOnEntityOnProfile.begin();

  for(int ii=0; ii<index; ii++)
    it++;

  return it->second;
}

void vtkMedFamilyOnEntity::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_OBJECT(os, indent, Family);
  PRINT_OBJECT(os, indent, EntityArray);
}
