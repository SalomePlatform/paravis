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
#include "vtkMedFile.h"

#include <sstream>

vtkCxxGetObjectVectorMacro(vtkMedMesh, CellFamily, vtkMedFamily);
vtkCxxSetObjectVectorMacro(vtkMedMesh, CellFamily, vtkMedFamily);
vtkCxxGetObjectVectorMacro(vtkMedMesh, PointFamily, vtkMedFamily);
vtkCxxSetObjectVectorMacro(vtkMedMesh, PointFamily, vtkMedFamily);

vtkCxxGetObjectVectorMacro(vtkMedMesh, PointGroup, vtkMedGroup);
vtkCxxSetObjectVectorMacro(vtkMedMesh, PointGroup, vtkMedGroup);
vtkCxxGetObjectVectorMacro(vtkMedMesh, CellGroup, vtkMedGroup);
vtkCxxSetObjectVectorMacro(vtkMedMesh, CellGroup, vtkMedGroup);

vtkCxxGetObjectVectorMacro(vtkMedMesh, AxisName, vtkMedString);
vtkCxxSetObjectVectorMacro(vtkMedMesh, AxisName, vtkMedString);
vtkCxxGetObjectVectorMacro(vtkMedMesh, AxisUnit, vtkMedString);
vtkCxxSetObjectVectorMacro(vtkMedMesh, AxisUnit, vtkMedString);

vtkCxxSetObjectMacro(vtkMedMesh, ParentFile, vtkMedFile);

vtkCxxRevisionMacro(vtkMedMesh, "$Revision$")
vtkStandardNewMacro(vtkMedMesh)

vtkMedMesh::vtkMedMesh()
{
  this->GridStep = new vtkMedComputeStepMap<vtkMedGrid> ();
  this->Name = vtkMedString::New();
  this->Name->SetSize(MED_NAME_SIZE);
  this->UniversalName = vtkMedString::New();
  this->UniversalName->SetSize(MED_LNAME_SIZE);
  this->Description = vtkMedString::New();
  this->Description->SetSize(MED_COMMENT_SIZE);
  this->TimeUnit = vtkMedString::New();
  this->TimeUnit->SetSize(MED_LNAME_SIZE);
  this->CellFamily = new vtkObjectVector<vtkMedFamily> ();
  this->PointFamily = new vtkObjectVector<vtkMedFamily> ();
  this->PointGroup = new vtkObjectVector<vtkMedGroup> ();
  this->CellGroup = new vtkObjectVector<vtkMedGroup> ();
  this->AxisName = new vtkObjectVector<vtkMedString> ();
  this->AxisUnit = new vtkObjectVector<vtkMedString> ();
  this->MedIterator = -1;
  this->MeshType = MED_UNDEF_MESH_TYPE;
  this->StructuredGridType = MED_UNDEF_GRID_TYPE;
  this->ParentFile = NULL;
}

vtkMedMesh::~vtkMedMesh()
{
  this->Name->Delete();
  this->UniversalName->Delete();
  this->Description->Delete();
  delete this->CellFamily;
  delete this->PointFamily;
  delete this->PointGroup;
  delete this->CellGroup;
  delete this->AxisName;
  delete this->AxisUnit;
  delete this->GridStep;
}

vtkMedGroup* vtkMedMesh::GetOrCreateGroup(int pointOrCell, const char* name)
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
    vtkMedGroup* group = vtkMedGroup::New();
    this->CellGroup->push_back(group);
    //group->SetPointOrCell(vtkMedUtilities::OnCell);
    group->GetName()->SetString(name);
    group->Delete();
    return group;
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
    vtkMedGroup* group = vtkMedGroup::New();
    this->CellGroup->push_back(group);
    //group->SetPointOrCell(vtkMedUtilities::OnPoint);
    group->GetName()->SetString(name);
    group->Delete();
    return group;
    }
  return NULL;
}

int vtkMedMesh::GetNumberOfFamily()
{
  return this->GetNumberOfCellFamily() + this->GetNumberOfPointFamily();
}

vtkMedFamily* vtkMedMesh::GetFamily(int index)
{
  if(index < 0)
    return NULL;
  if(index < this->GetNumberOfCellFamily())
    return this->GetCellFamily(index);
  else if(index < GetNumberOfFamily())
    return this->GetPointFamily(index - this->GetNumberOfCellFamily());
  else return NULL;
}

vtkMedFamily* vtkMedMesh::GetOrCreateCellFamilyById(med_int id)
{
  for(int i = 0; i < this->GetNumberOfCellFamily(); i++)
    {
    vtkMedFamily* family = this->GetCellFamily(i);
    if(family->GetId() == id)
      {
      return family;
      }
    }
  vtkMedFamily* family = vtkMedFamily::New();
  family->SetId(id);
  vtkstd::ostringstream sstr;
  sstr << "UNDEFINED_CELL_FAMILY_" << id;
  family->GetName()->SetString(sstr.str().c_str());
  family->SetPointOrCell(vtkMedUtilities::OnCell);
  family->SetMedIterator(-1);
  this->AppendCellFamily(family);
  family->Delete();
  return family;
}

void  vtkMedMesh::SetNumberOfAxis(int naxis)
{
  this->AllocateNumberOfAxisName(naxis);
  this->AllocateNumberOfAxisUnit(naxis);
  for(int axis = 0; axis < naxis; axis++)
    {
    this->AxisName->at(axis)->SetSize(MED_SNAME_SIZE);
    this->AxisUnit->at(axis)->SetSize(MED_SNAME_SIZE);
    }
}

int  vtkMedMesh::GetNumberOfAxis()
{
  return this->AxisName->size();
}

vtkMedFamily* vtkMedMesh::GetOrCreatePointFamilyById(med_int id)
{
  for(int i = 0; i < this->GetNumberOfPointFamily(); i++)
    {
    vtkMedFamily* family = this->GetPointFamily(i);

    if(family->GetId() == id)
      return family;
    }
  vtkMedFamily* family = vtkMedFamily::New();
  family->SetId(id);
  vtkstd::ostringstream sstr;
  sstr << "UNDEFINED_POINT_FAMILY_" << id;
  family->GetName()->SetString(sstr.str().c_str());
  family->SetPointOrCell(vtkMedUtilities::OnPoint);
  this->AppendPointFamily(family);
  family->Delete();
  return family;
}

void  vtkMedMesh::AddGridStep(vtkMedGrid* grid)
{
  this->GridStep->AddObject(grid->GetComputeStep(), grid);
}

void  vtkMedMesh::ClearGridStep()
{
  this->GridStep->clear();
}

vtkMedGrid* vtkMedMesh::GetGridStep(const vtkMedComputeStep& cs)
{
  return this->GridStep->GetObject(cs);
}

vtkMedGrid* vtkMedMesh::FindGridStep(const vtkMedComputeStep& cs,
                                     int strategy)
{
  return this->GridStep->FindObject(cs, strategy);
}

void  vtkMedMesh::GatherGridTimes(std::set<med_float>& timeset)
{
  this->GridStep->GatherTimes(timeset);
}

void  vtkMedMesh::GatherGridIterations(med_float time,
                                       std::set<med_int>& iterationset)
{
  this->GridStep->GatherIterations(time, iterationset);
}

void  vtkMedMesh::ClearMedSupports()
{
  med_int stepnb = this->GridStep->GetNumberOfObject();
  for(med_int stepid = 0; stepid<stepnb; stepid++ )
    {
    vtkMedGrid* grid = this->GridStep->GetObject(stepid);
    grid->ClearMedSupports();
    }
}

med_int vtkMedMesh::GetNumberOfGridStep()
{
  return this->GridStep->GetNumberOfObject();
}

vtkMedGrid* vtkMedMesh::GetGridStep(med_int id)
{
  return this->GridStep->GetObject(id);
}

void vtkMedMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_IVAR(os, indent, MedIterator);
  PRINT_MED_STRING(os, indent, Name);
  PRINT_MED_STRING(os, indent, UniversalName);
  PRINT_MED_STRING(os, indent, Description);
  PRINT_OBJECT_VECTOR(os, indent, CellFamily);
  PRINT_OBJECT_VECTOR(os, indent, PointFamily);
  PRINT_OBJECT_VECTOR(os, indent, PointGroup);
  PRINT_OBJECT_VECTOR(os, indent, CellGroup);
  PRINT_OBJECT_VECTOR(os, indent, AxisName);
  PRINT_OBJECT_VECTOR(os, indent, AxisUnit);
}
