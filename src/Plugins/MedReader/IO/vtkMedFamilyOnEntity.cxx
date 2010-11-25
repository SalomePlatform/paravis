#include "vtkMedFamilyOnEntity.h"

#include "vtkObjectFactory.h"
#include "vtkMedUtilities.h"
#include "vtkMedFamily.h"
#include "vtkMedEntityArray.h"
#include "vtkMedMesh.h"

vtkCxxSetObjectMacro(vtkMedFamilyOnEntity, Family, vtkMedFamily)
vtkCxxSetObjectMacro(vtkMedFamilyOnEntity, EntityArray, vtkMedEntityArray)
vtkCxxSetObjectMacro(vtkMedFamilyOnEntity, Mesh, vtkMedMesh)

vtkCxxRevisionMacro(vtkMedFamilyOnEntity, "$Revision$")
vtkStandardNewMacro(vtkMedFamilyOnEntity)

vtkMedFamilyOnEntity::vtkMedFamilyOnEntity()
{
  this->Family = NULL;
  this->Mesh = NULL;
  this->EntityArray = NULL;
  this->AllPoints = -1;
}

vtkMedFamilyOnEntity::~vtkMedFamilyOnEntity()
{
  this->SetFamily(NULL);
  this->SetEntityArray(NULL);
  this->SetMesh(NULL);
}

med_entite_maillage vtkMedFamilyOnEntity::GetType()
{
  if(this->EntityArray != NULL)
    return this->EntityArray->GetType();
  return MED_NOEUD;
}

med_geometrie_element vtkMedFamilyOnEntity::GetGeometry()
{
  if(this->EntityArray != NULL)
    return this->EntityArray->GetGeometry();
  return MED_NONE;
}

int vtkMedFamilyOnEntity::GetPointOrCell()
{
  if(this->GetType() == MED_NOEUD)
    return vtkMedUtilities::OnPoint;
  return vtkMedUtilities::OnCell;
}

int vtkMedFamilyOnEntity::GetVertexOnly()
{
  return this->GetPointOrCell() == vtkMedUtilities::OnPoint
      || this->EntityArray == NULL || this->EntityArray->GetGeometry()
      == MED_POINT1 || this->EntityArray->GetGeometry() == MED_NONE;
}

void vtkMedFamilyOnEntity::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  PRINT_OBJECT(os, indent, Family);
  PRINT_OBJECT(os, indent, EntityArray);
  os << indent << "Mesh : " << this->Mesh << endl;
}
