#include "vtkMedUtilities.h"

#include "vtkMedMesh.h"
#include "vtkMedFamily.h"
#include "vtkMedGroup.h"
#include "vtkMedString.h"

#include "vtkObjectFactory.h"
#include "vtkDoubleArray.h"
#include "vtkIntArray.h"
#include "vtkLongLongArray.h"
#include "vtkLongArray.h"
#include "vtkIdTypeArray.h"
#include "vtkCellType.h"
#include "vtkIdList.h"

#include <sstream>
using namespace std;

char vtkMedUtilities::Separator = '/';

const char* vtkMedUtilities::NoGroupName = "No Group";
const char* vtkMedUtilities::OnPointName = "OnPoint";
const char* vtkMedUtilities::OnCellName = "OnCell";

const med_geometrie_element
    vtkMedUtilities::CellGeometry[vtkMedUtilities::NumberOfCellGeometry] = {
        MED_POINT1, MED_SEG2, MED_SEG3, MED_TRIA3, MED_QUAD4, MED_TRIA6,
        MED_QUAD8, MED_TETRA4, MED_PYRA5, MED_PENTA6, MED_HEXA8, MED_TETRA10,
        MED_PYRA13, MED_PENTA15, MED_HEXA20, MED_POLYGONE, MED_POLYEDRE,
        MED_NONE };

const med_geometrie_element
    vtkMedUtilities::FaceGeometry[vtkMedUtilities::NumberOfFaceGeometry] = {
        MED_TRIA3, MED_QUAD4, MED_TRIA6, MED_QUAD8, MED_POLYGONE };

const med_geometrie_element
    vtkMedUtilities::EdgeGeometry[vtkMedUtilities::NumberOfEdgeGeometry] = {
        MED_SEG2, MED_SEG3 };

const med_geometrie_element
    vtkMedUtilities::VertexGeometry[vtkMedUtilities::NumberOfVertexGeometry] = {
        MED_POINT1 };

const med_connectivite
    vtkMedUtilities::Connectivity[vtkMedUtilities::NumberOfConnectivity] = {
        MED_NOD, MED_DESC };

const med_entite_maillage
    vtkMedUtilities::EntityType[vtkMedUtilities::NumberOfEntityType] = {
        MED_MAILLE, MED_FACE, MED_ARETE, MED_NOEUD, MED_NOEUD_MAILLE };

const int MED_TRIA_CHILD_TO_PARENT_INDEX[3][3] = { { 0, 1, 3 }, { 1, 2, 4 }, {
    2, 0, 5 } };

const int MED_QUAD_CHILD_TO_PARENT_INDEX[4][3] = { { 0, 1, 4 }, { 1, 2, 5 }, {
    2, 3, 6 }, { 3, 0, 7 } };

const int MED_TETRA_CHILD_TO_PARENT_INDEX[4][6] = { { 0, 1, 2, 4, 5, 6 }, { 0,
    3, 1, 7, 8, 4 }, { 1, 3, 2, 8, 9, 5 }, { 2, 3, 0, 9, 7, 6 } };

const int MED_HEXA_CHILD_TO_PARENT_INDEX[6][8] = {
    { 0, 1, 2, 3, 8, 9, 10, 11 }, { 4, 7, 6, 5, 15, 14, 13, 12 }, { 0, 4, 5, 1,
        16, 12, 17, 8 }, { 1, 5, 6, 2, 17, 13, 18, 9 }, { 2, 6, 7, 3, 18, 14,
        19, 10 }, { 3, 7, 4, 0, 19, 15, 16, 11 } };

const int MED_PYRA_CHILD_TO_PARENT_INDEX[5][8] = { { 0, 1, 2, 3, 5, 6, 7, 8 },
    { 0, 4, 1, -1, 9, 10, 5, -1 }, { 1, 4, 2, -1, 10, 11, 6, -1 }, { 2, 4, 3,
        -1, 11, 12, 7, -1 }, { 3, 4, 0, -1, 12, 9, 8, -1 } };

const int MED_PENTA_CHILD_TO_PARENT_INDEX[5][8] = {
    { 0, 1, 2, -1, 6, 7, 8, -1 }, { 3, 5, 4, -1, 11, 10, 9, -1 }, { 0, 3, 4, 1,
        12, 9, 13, 6 }, { 1, 4, 5, 2, 13, 10, 14, 7 }, { 2, 5, 3, 0, 14, 11,
        12, 8 } };

vtkCxxRevisionMacro(vtkMedUtilities, "$Revision$")
vtkStandardNewMacro(vtkMedUtilities)

vtkDataArray* vtkMedUtilities::NewCoordArray()
{
  return vtkMedUtilities::NewArray(MED_FLOAT64);
}

vtkDataArray* vtkMedUtilities::NewArray(med_type_champ type)
{
  switch(type)
    {
    case MED_FLOAT64:
      if(sizeof(double) == 8)
        return vtkDoubleArray::New();
      vtkGenericWarningMacro("double type do not match MED_FLOAT64, aborting")
      return NULL;
    case MED_INT32:
      if(sizeof(vtkIdType) == 4)
        return vtkIdTypeArray::New();
      if(sizeof(int) == 4)
        return vtkIntArray::New();
      if(sizeof(long) == 4)
        return vtkLongArray::New();
      vtkGenericWarningMacro("No vtk type matches MED_INT32, aborting")
      return NULL;
    case MED_INT64:
      if(sizeof(vtkIdType) == 8)
        return vtkIdTypeArray::New();
      if(sizeof(long) == 8)
        return vtkLongArray::New();
      if(sizeof(long long) == 8)
        return vtkLongLongArray::New();
      vtkGenericWarningMacro("No vtk type matches MED_INT64, aborting")
      ;
      return NULL;
    case MED_INT:
      if(sizeof(med_int) == 4)
        return vtkMedUtilities::NewArray(MED_INT32);
      if(sizeof(med_int) == 8)
        return vtkMedUtilities::NewArray(MED_INT64);
      vtkGenericWarningMacro("No vtk type matches MED_INT, aborting")
      return NULL;
    default:
      vtkGenericWarningMacro("the array type is not known, aborting.")
      return NULL;
    }
}

const char* vtkMedUtilities::GeometryName(med_geometrie_element geometry)
{
  switch(geometry)
    {
    case MED_POINT1:
      return "MED_POINT1";
    case MED_SEG2:
      return "MED_SEG2";
    case MED_SEG3:
      return "MED_SEG3";
    case MED_TRIA3:
      return "MED_TRIA3";
    case MED_QUAD4:
      return "MED_QUAD4";
    case MED_TRIA6:
      return "MED_TRIA6";
    case MED_QUAD8:
      return "MED_QUAD8";
    case MED_TETRA4:
      return "MED_TETRA4";
    case MED_PYRA5:
      return "MED_PYRA5";
    case MED_PENTA6:
      return "MED_PENTA6";
    case MED_HEXA8:
      return "MED_HEXA8";
    case MED_TETRA10:
      return "MED_TETRA10";
    case MED_PYRA13:
      return "MED_PYRA13";
    case MED_PENTA15:
      return "MED_PENTA15";
    case MED_HEXA20:
      return "MED_HEXA20";
    case MED_POLYGONE:
      return "MED_POLYGONE";
    case MED_POLYEDRE:
      return "MED_POLYEDRE";
    case MED_NONE:
      return "MED_NONE";
    default:
      return "UNKNOWN_GEOMETRY";
    }
}

const char* vtkMedUtilities::TypeName(med_entite_maillage type)
{
  switch(type)
    {
    case MED_MAILLE:
      return "MED_MAILLE";
    case MED_FACE:
      return "MED_FACE";
    case MED_ARETE:
      return "MED_ARETE";
    case MED_NOEUD:
      return "MED_NOEUD";
    case MED_NOEUD_MAILLE:
      return "MED_NOEUD_MAILLE";
    default:
      return "UNKNOWN_ENTITY_TYPE";
    }
}

const char* vtkMedUtilities::ConnectivityName(med_connectivite conn)
{
  switch(conn)
    {
    case MED_NOD:
      return "MED_NOD";
    case MED_DESC:
      return "MED_DESC";
    default:
      return "UNKNOWN_CONNECTIVITY";
    }
}

const std::string vtkMedUtilities::SimplifyName(const vtkMedString* medString)
{
  return vtkMedUtilities::SimplifyName(medString->GetString());
}

const std::string vtkMedUtilities::SimplifyName(const char* medName)
{
  ostringstream sstr;
  bool underscore = false;
  bool space = false;
  int l = strlen(medName);
  for(int cc = 0; cc < l; cc++)
    {
    if(medName[cc] == ' ')
      {
      space = true;
      continue;
      }
    else if(medName[cc] == '_')
      {
      underscore = true;
      continue;
      }
    else
      {
      if(underscore || space)
        sstr << '_';
      underscore = false;
      space = false;
      sstr << medName[cc];
      }
    }
  return sstr.str();
}

const std::string vtkMedUtilities::FamilyKey(const char* meshName,
    int pointOrCell, const char* familyName)
{
  ostringstream sstr;
  sstr << SimplifyName(meshName) << Separator;
  if(pointOrCell == OnCell)
    sstr << vtkMedUtilities::OnCellName;
  else
    sstr << vtkMedUtilities::OnPointName;
  sstr << Separator << SimplifyName(familyName);
  return sstr.str();
}

const std::string vtkMedUtilities::GroupKey(const char* meshName,
    int pointOrCell, const char* groupName)
{
  ostringstream sstr;
  sstr << SimplifyName(meshName) << Separator;
  if(pointOrCell == OnCell)
    sstr << vtkMedUtilities::OnCellName;
  else
    sstr << vtkMedUtilities::OnPointName;
  if(groupName == NULL)
    sstr << Separator << NoGroupName;
  else
    sstr << Separator << SimplifyName(groupName);
  return sstr.str();
}

const std::string vtkMedUtilities::CellTypeKey(med_entite_maillage type,
    med_geometrie_element geometry)
{
  ostringstream sstr;
  sstr << TypeName(type) << Separator << GeometryName(geometry);
  return sstr.str();
}

int vtkMedUtilities::GetNumberOfPoint(med_geometrie_element geometry)
{
  return geometry % 100;
}

int vtkMedUtilities::GetDimension(med_geometrie_element geometry)
{
  return geometry / 100;
}

int vtkMedUtilities::GetVTKCellType(med_geometrie_element geometry)
{

  switch(geometry)
    {
    case MED_POINT1:
      return VTK_VERTEX;
    case MED_SEG2:
      return VTK_LINE;
    case MED_SEG3:
      return VTK_QUADRATIC_EDGE;
    case MED_TRIA3:
      return VTK_TRIANGLE;
    case MED_QUAD4:
      return VTK_QUAD;
    case MED_TRIA6:
      return VTK_QUADRATIC_TRIANGLE;
    case MED_QUAD8:
      return VTK_QUADRATIC_QUAD;
    case MED_TETRA4:
      return VTK_TETRA;
    case MED_PYRA5:
      return VTK_PYRAMID;
    case MED_PENTA6:
      return VTK_WEDGE;
    case MED_HEXA8:
      return VTK_HEXAHEDRON;
    case MED_TETRA10:
      return VTK_QUADRATIC_TETRA;
    case MED_PYRA13:
      return VTK_QUADRATIC_PYRAMID;
    case MED_PENTA15:
      return VTK_QUADRATIC_WEDGE;
    case MED_HEXA20:
      return VTK_QUADRATIC_HEXAHEDRON;
    case MED_POLYGONE:
      return VTK_POLYGON;
    case MED_POLYEDRE:
      return VTK_CONVEX_POINT_SET;
    case MED_NONE:
      return VTK_EMPTY_CELL;
    default:
      vtkGenericWarningMacro("No vtk type matches " << vtkMedUtilities::GeometryName(geometry) << ", aborting")
      ;
      return VTK_EMPTY_CELL;
    }
}

int vtkMedUtilities::GetNumberOfSubEntity(med_geometrie_element geometry)
{
  switch(geometry)
    {
    case MED_POINT1:
      return 0;
    case MED_SEG2:
      return 2;
    case MED_SEG3:
      return 3;
    case MED_TRIA3:
      return 3;
    case MED_QUAD4:
      return 4;
    case MED_TRIA6:
      return 3;
    case MED_QUAD8:
      return 4;
    case MED_TETRA4:
      return 4;
    case MED_PYRA5:
      return 5;
    case MED_PENTA6:
      return 5;
    case MED_HEXA8:
      return 6;
    case MED_TETRA10:
      return 4;
    case MED_PYRA13:
      return 5;
    case MED_PENTA15:
      return 5;
    case MED_HEXA20:
      return 6;
    case MED_POLYGONE:
      return 0;
    case MED_POLYEDRE:
      return 0;
    case MED_NONE:
      return 0;
    default:
      vtkGenericWarningMacro("No vtk type matches " << vtkMedUtilities::GeometryName(geometry) << ", aborting")
      ;
      return -1;
    }
}

med_entite_maillage vtkMedUtilities::GetSubType(med_entite_maillage type)
{
  switch(type)
    {
    case MED_MAILLE:
      return MED_FACE;
    case MED_FACE:
      return MED_ARETE;
    case MED_ARETE:
      return MED_NOEUD;
    default:
      return MED_NOEUD;
    }
}

med_geometrie_element vtkMedUtilities::GetSubGeometry(
    med_geometrie_element geometry, int index)
{
  switch(geometry)
    {
    case MED_SEG2:
      return MED_POINT1;
    case MED_SEG3:
      return MED_POINT1;
    case MED_TRIA3:
      return MED_SEG2;
    case MED_QUAD4:
      return MED_SEG2;
    case MED_TRIA6:
      return MED_SEG3;
    case MED_QUAD8:
      return MED_SEG3;
    case MED_TETRA4:
      return MED_TRIA3;
    case MED_PYRA5:
      {
      if(index == 0)
        return MED_QUAD4;
      return MED_TRIA3;
      }
    case MED_PENTA6:
      {
      if(index == 0 || index == 1)
        return MED_TRIA3;
      else
        return MED_QUAD4;
      }
    case MED_HEXA8:
      return MED_QUAD4;
    case MED_TETRA10:
      return MED_TRIA6;
    case MED_PYRA13:
      {
      if(index == 0)
        return MED_QUAD8;
      else
        return MED_TRIA6;
      }
    case MED_PENTA15:
      {
      if(index == 0 || index == 1)
        return MED_TRIA6;
      else
        return MED_QUAD8;
      }
    case MED_HEXA20:
      return MED_QUAD8;
    default:
      return MED_NONE;
    }
}

void vtkMedUtilities::SplitGroupKey(const char* name, vtkstd::string& mesh,
    vtkstd::string& entity, vtkstd::string& group)
{
  vtkstd::string remain;
  remain = name;
  mesh = "*";
  entity = "*";
  group = "*";

  if(remain == "*")
    {
    return;
    }
  vtkstd::string::size_type pos;
  pos = remain.find_first_of(vtkMedUtilities::Separator);
  mesh = remain.substr(0, pos);
  if(mesh == "*" || pos == remain.size() - 1)
    return;
  remain = remain.substr(pos + 1, remain.size() - pos - 1);
  pos = remain.find_first_of(vtkMedUtilities::Separator);
  entity = remain.substr(0, pos);
  if(entity == "*" || pos == remain.size() - 1)
    return;
  group = remain.substr(pos + 1, remain.size() - pos - 1);
}

int vtkMedUtilities::GetParentNodeIndex(med_geometrie_element parentGeometry,
    int subEntityIndex, int subEntityNodeIndex)
{
  switch(parentGeometry)
    {
    case MED_TRIA3:
    case MED_TRIA6:
      return MED_TRIA_CHILD_TO_PARENT_INDEX[subEntityIndex][subEntityNodeIndex];
    case MED_QUAD4:
    case MED_QUAD8:
      return MED_QUAD_CHILD_TO_PARENT_INDEX[subEntityIndex][subEntityNodeIndex];
    case MED_TETRA4:
    case MED_TETRA10:
      return MED_TETRA_CHILD_TO_PARENT_INDEX[subEntityIndex][subEntityNodeIndex];
    case MED_PYRA5:
    case MED_PYRA13:
      return MED_PYRA_CHILD_TO_PARENT_INDEX[subEntityIndex][subEntityNodeIndex];
    case MED_PENTA6:
    case MED_PENTA15:
      return MED_PENTA_CHILD_TO_PARENT_INDEX[subEntityIndex][subEntityNodeIndex];
    case MED_HEXA8:
    case MED_HEXA20:
      return MED_HEXA_CHILD_TO_PARENT_INDEX[subEntityIndex][subEntityNodeIndex];
    }
  return -1;
}

void vtkMedUtilities::ProjectConnectivity(med_geometrie_element parentGeometry,
    vtkIdList* parentIds, vtkIdList* subEntityIds, int subEntityIndex)
{
  for(int subEntityNodeIndex = 0; subEntityNodeIndex
      < subEntityIds->GetNumberOfIds(); subEntityNodeIndex++)
    {
    parentIds->SetId(GetParentNodeIndex(parentGeometry, subEntityIndex,
        subEntityNodeIndex), subEntityIds->GetId(subEntityNodeIndex));
    }
}

void vtkMedUtilities::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

