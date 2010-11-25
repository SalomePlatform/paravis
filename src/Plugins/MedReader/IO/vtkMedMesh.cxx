#include "vtkMedMesh.h"

#include "vtkObjectFactory.h"
#include "vtkSmartPointer.h"
#include "vtkDataArray.h"

#include "vtkMedFamily.h"
#include "vtkMedFamilyOnEntity.h"
#include "vtkMedGroup.h"
#include "vtkMedGrid.h"
#include "vtkMedUtilities.h"
#include "vtkMedString.h"
#include "vtkMedIntArray.h"

#include <set>
#include <sstream>
using std::set;

vtkCxxGetObjectVectorMacro(vtkMedMesh, Family, vtkMedFamily)
vtkCxxSetObjectVectorMacro(vtkMedMesh, Family, vtkMedFamily)

vtkCxxGetObjectVectorMacro(vtkMedMesh, PointGroup, vtkMedGroup)
vtkCxxSetObjectVectorMacro(vtkMedMesh, PointGroup, vtkMedGroup)
vtkCxxGetObjectVectorMacro(vtkMedMesh, CellGroup, vtkMedGroup)
vtkCxxSetObjectVectorMacro(vtkMedMesh, CellGroup, vtkMedGroup)

vtkCxxGetObjectVectorMacro(vtkMedMesh, PointFamilyOnEntity, vtkMedFamilyOnEntity)
vtkCxxSetObjectVectorMacro(vtkMedMesh, PointFamilyOnEntity, vtkMedFamilyOnEntity)

vtkCxxSetObjectMacro(vtkMedMesh,Grid,vtkMedGrid)
vtkCxxSetObjectMacro(vtkMedMesh,PointGlobalIds,vtkMedIntArray)
vtkCxxSetObjectMacro(vtkMedMesh,PointFamilyIds,vtkMedIntArray)

vtkCxxRevisionMacro(vtkMedMesh, "$Revision$")
vtkStandardNewMacro(vtkMedMesh)

vtkMedMesh::vtkMedMesh()
{
  this->Grid = NULL;
  this->Name = vtkMedString::New();
  this->Name->SetSize(MED_TAILLE_NOM);
  this->UniversalName = vtkMedString::New();
  this->UniversalName->SetSize(MED_TAILLE_LNOM);
  this->Description = vtkMedString::New();
  this->Description->SetSize(MED_TAILLE_DESC);
  this->PointGlobalIds = NULL;
  this->PointFamilyIds = NULL;
  this->Family = new vtkObjectVector<vtkMedFamily> ();
  this->PointGroup = new vtkObjectVector<vtkMedGroup> ();
  this->CellGroup = new vtkObjectVector<vtkMedGroup> ();
  this->PointFamilyOnEntity = new vtkObjectVector<vtkMedFamilyOnEntity> ();
  this->MedIndex = -1;
}

vtkMedMesh::~vtkMedMesh()
{
  this->Name->Delete();
  this->UniversalName->Delete();
  this->Description->Delete();
  this->SetGrid(NULL);
  this->SetPointGlobalIds(NULL);
  this->SetPointFamilyIds(NULL);
  delete this->Family;
  delete this->PointGroup;
  delete this->CellGroup;
  delete this->PointFamilyOnEntity;
}

int vtkMedMesh::IsPointGlobalIdsLoaded()
{
  return this->PointGlobalIds != NULL && this->Grid != NULL
      && this->PointGlobalIds->GetNumberOfTuples()
          == this->Grid->GetNumberOfPoints();
}

int vtkMedMesh::IsPointFamilyIdsLoaded()
{
  return this->PointFamilyIds != NULL && this->Grid != NULL
      && this->PointFamilyIds->GetNumberOfTuples()
          == this->Grid->GetNumberOfPoints();
}

vtkMedGroup* vtkMedMesh::GetGroup(int pointOrCell, const char* name)
{
  if(pointOrCell == vtkMedUtilities::OnCell)
    {
    for(int g = 0; g < this->CellGroup->size(); g++)
      {
      vtkMedGroup* group = this->CellGroup->at(g);
      if(group != NULL && strcmp(name, group->GetName()->GetString()) == 0)
        {
        return group;
        }
      }
    }
  else
    {
    for(int g = 0; g < this->PointGroup->size(); g++)
      {
      vtkMedGroup* group = this->PointGroup->at(g);
      if(group != NULL && strcmp(name, group->GetName()->GetString()) == 0)
        {
        return group;
        }
      }
    }
  return NULL;
}

void vtkMedMesh::ComputePointFamilies()
{
  this->PointFamilyOnEntity->clear();
  set<med_int> famIds;
  if(this->PointFamilyIds == NULL)
    {
    vtkMedFamilyOnEntity* foe = vtkMedFamilyOnEntity::New();
    this->AppendPointFamilyOnEntity(foe);
    foe->Delete();
    foe->SetFamily(this->GetPointFamilyOnEntityById(0)->GetFamily());
    return;
    }
  for(vtkIdType index = 0; index < this->PointFamilyIds->GetNumberOfTuples(); index++)
    {
    famIds.insert(this->PointFamilyIds->GetValue(index));
    }
  set<med_int>::iterator it = famIds.begin();
  while(it != famIds.end())
    {
    vtkMedFamilyOnEntity* foe = vtkMedFamilyOnEntity::New();
    foe->SetMesh(this);
    this->AppendPointFamilyOnEntity(foe);
    foe->Delete();
    foe->SetFamily(this->GetOrCreatePointFamilyById(*it));
    it++;
    }
}

int vtkMedMesh::HasPointFamily(vtkMedFamily* family)
{
  if(this->PointFamilyIds == NULL && family->GetId() == 0)
    return 1;
  for(vtkObjectVector<vtkMedFamilyOnEntity>::iterator it =
      this->PointFamilyOnEntity->begin(); it
      != this->PointFamilyOnEntity->end(); it++)
    {
    if((*it)->GetFamily() == family)
      return 1;
    }
  return 0;
}

vtkMedFamilyOnEntity* vtkMedMesh::GetPointFamilyOnEntityById(med_int id)
{
  for(int i = 0; i < this->GetNumberOfPointFamilyOnEntity(); i++)
    {
    vtkMedFamilyOnEntity* foe = this->GetPointFamilyOnEntity(i);
    if(foe->GetFamily()->GetId() == id)
      return foe;
    }
  return NULL;
}

vtkMedFamily* vtkMedMesh::GetOrCreatePointFamilyById(med_int id)
{
  for(int i = 0; i < this->GetNumberOfFamily(); i++)
    {
    vtkMedFamily* family = this->GetFamily(i);
    if(family->GetPointOrCell() != vtkMedUtilities::OnPoint)
      continue;
    if(family->GetId() == id)
      return family;
    }
  vtkMedFamily* family = vtkMedFamily::New();
  family->SetId(id);
  vtkstd::ostringstream sstr;
  sstr << "UNDEFINED_POINT_FAMILY_" << id;
  family->GetName()->SetString(sstr.str().c_str());
  family->SetPointOrCell(vtkMedUtilities::OnPoint);
  family->SetMedIndex(-1);
  this->AppendFamily(family);
  family->Delete();
  return family;
}

vtkMedFamily* vtkMedMesh::GetOrCreateCellFamilyById(med_int id)
{
  for(int i = 0; i < this->GetNumberOfFamily(); i++)
    {
    vtkMedFamily* family = this->GetFamily(i);
    if(family->GetPointOrCell() != vtkMedUtilities::OnCell)
      continue;
    if(family->GetId() == id)
      return family;
    }
  vtkMedFamily* family = vtkMedFamily::New();
  family->SetId(id);
  vtkstd::ostringstream sstr;
  sstr << "UNDEFINED_CELL_FAMILY_" << id;
  family->GetName()->SetString(sstr.str().c_str());
  family->SetPointOrCell(vtkMedUtilities::OnCell);
  family->SetMedIndex(-1);
  this->AppendFamily(family);
  family->Delete();
  return family;
}

int vtkMedMesh::GetNumberOfCellFamily()
{
  int nb = 0;
  for(int i = 0; i < this->GetNumberOfFamily(); i++)
    {
    vtkMedFamily* family = this->GetFamily(i);
    if(family->GetPointOrCell() == vtkMedUtilities::OnCell)
      nb++;
    }
  return nb;
}

void vtkMedMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_IVAR(os, indent, MedIndex);
  PRINT_MED_STRING(os, indent, Name);
  PRINT_MED_STRING(os, indent, UniversalName);
  PRINT_MED_STRING(os, indent, Description);
  PRINT_OBJECT_VECTOR(os, indent, Family);
  PRINT_OBJECT_VECTOR(os, indent, PointGroup);
  PRINT_OBJECT_VECTOR(os, indent, CellGroup);
  PRINT_OBJECT(os, indent, Grid);
}
