#ifndef _vtkMedUtilities_h_
#define _vtkMedUtilities_h_

#include "vtkObject.h"
#include "vtkSmartPointer.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

#include <utility>
#include <string>
#include <vector>
#include <list>

class vtkDataArray;
class vtkMedString;
class vtkMedMesh;
class vtkMedFamily;
class vtkMedGroup;
class vtkIdList;
class vtkMutableDirectedGraph;

class VTK_EXPORT vtkMedUtilities: public vtkObject
{
public:
  static vtkMedUtilities* New();
vtkTypeRevisionMacro(vtkMedUtilities, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // return an array to store the coordinates of nodes.
  // the  type of the elements is the same as the one in the med file
  static vtkDataArray* NewCoordArray();

  // Description:
  // returns an array to store data of a given type.
  // the type corresponds to med types.
  static vtkDataArray* NewArray(med_type_champ type);

  //BTX
  enum
  {
    OnPoint, OnCell
  };
  //ETX

  //BTX
  // Description:
  // returns the ith med_geometrie_element
  static const int NumberOfCellGeometry=MED_NBR_GEOMETRIE_MAILLE+3;
  static const med_geometrie_element CellGeometry[NumberOfCellGeometry];
  static const int NumberOfFaceGeometry=MED_NBR_GEOMETRIE_FACE+1;
  static const med_geometrie_element FaceGeometry[NumberOfFaceGeometry];
  static const int NumberOfEdgeGeometry=MED_NBR_GEOMETRIE_ARETE;
  static const med_geometrie_element EdgeGeometry[NumberOfEdgeGeometry];
  static const int NumberOfVertexGeometry=1;
  static const med_geometrie_element VertexGeometry[NumberOfVertexGeometry];

  // Description:
  // returns a name for the given med_geometrie_element
  static const char* GeometryName(med_geometrie_element geometry);

  // Description:
  // returns a name for the given med_geometrie_element
  static const char* TypeName(med_entite_maillage type);

  // Description:
  // returns a name for the given med_connectivite
  static const char* ConnectivityName(med_connectivite conn);

  // Description:
  // returns the ith med_connectivite
  static const int NumberOfConnectivity=2;
  static const med_connectivite Connectivity[NumberOfConnectivity];

  // Description:
  // returns the ith med_entite_maillage
  static const int NumberOfEntityType=5;
  static const med_entite_maillage EntityType[NumberOfEntityType];

  static const std::string SimplifyName(const char* medName);
  static const std::string SimplifyName(const vtkMedString*);

  static const std::string FamilyKey(const char* meshName, int pointOrCell,
      const char* familyName);
  static const std::string GroupKey(const char* meshName, int pointOrCell,
      const char* groupName);

  static const std::string
  CellTypeKey(med_entite_maillage type, med_geometrie_element geometry);

  static int GetNumberOfPoint(med_geometrie_element geometry);
  static int GetDimension(med_geometrie_element geometry);

  // returns the VTK cell type (as described in the vtkCellType.h file)
  // corresponding to the given med_geometrie_element
  static int GetVTKCellType(med_geometrie_element geometry);

  // returns the number of sub entity : the number of faces for cells,
  // the number of edges for faces, the number of nodes for edges
  static int GetNumberOfSubEntity(med_geometrie_element geometry);
  //ETX

  static med_entite_maillage GetSubType(med_entite_maillage type);
  static med_geometrie_element GetSubGeometry(med_geometrie_element geometry,
      int index);

  static int GetParentNodeIndex(med_geometrie_element parentGeometry,
      int subEntityIndex, int subEntityNodeIndex);

  // Description :
  // Project the ids gathered in the sub entity to the parent entity.
  // used for MED_DESC connectivity.
  // Rem : no check is performed, and do not work for MED_POLYHEDRE
  // and MED_POLYGON
  static void ProjectConnectivity(med_geometrie_element parentGeometry,
      vtkIdList* parentIds, vtkIdList* subEntityIds, int subEntityIndex);

  static char Separator;

  static const char* NoGroupName;
  static const char* OnCellName;
  static const char* OnPointName;

  //BTX
  static void SplitGroupKey(const char* name, vtkstd::string& mesh,
      vtkstd::string& entity, vtkstd::string& group);
  //ETX
};

//BTX

template<class T>
class vtkObjectVector: public std::vector<vtkSmartPointer<T> >
{
};

template<class T>
class vtkList: public std::list<T>
{
};

template<class T1, class T2>
struct IsSameTraits
{
  static const bool IsSame()
  {
    return false;
  }
};

template<class T1>
struct IsSameTraits<T1, T1>
{
  static const bool IsSame()
  {
    return true;
  }
};

typedef std::pair<med_entite_maillage, med_geometrie_element> Entity;

#define PRINT_IVAR(os, indent, name) \
	os << indent << #name << " : "  << name << endl;

#define PRINT_STRING(os, indent, name) \
	os << indent << #name << " : "  << ( name ? name : "(void)") << endl;

#define PRINT_OBJECT(os, indent, name) \
	os << indent << #name << " : " ;\
	if(name != NULL) \
	{\
		os << endl;\
		name->PrintSelf(os, indent.GetNextIndent());\
	}\
	else os << "(NULL)" << endl;

#define PRINT_VECTOR(os, indent, name, size) \
{\
	os << indent << #name << " : (";\
	for(vtkIdType _index = 0; _index<size; _index++)\
		{\
		os << name[_index];\
		if(_index < size-1)\
			os << ", ";\
		}\
	os << ")" << endl;\
}

#define PRINT_OBJECT_VECTOR(os, indent, name) \
{\
	os << indent << #name;\
	os << endl;\
	vtkIdType _size = name->size();\
	for(vtkIdType _index = 0; _index < _size; _index++)\
	{\
	os << indent << #name << _index << " : " << endl;\
	if(name->at(_index) != NULL)\
		name->at(_index)->PrintSelf(os, indent.GetNextIndent());\
	else\
		os << indent.GetNextIndent() << "(NULL)" << endl;\
	}\
}

#define PRINT_STRING_VECTOR(os, indent, name)\
{\
	os << indent << #name << ": ";\
	for(int _comp = 0; _comp<this->name->size(); _comp++)\
		{\
		os << this->name->at(_comp)->GetString();\
		if(_comp < this->name->size()-1)\
			os << ", ";\
		}\
	os << endl;\
}

#define PRINT_MED_STRING(os, indent, name)\
	os << indent << #name << ": " << this->name->GetString() << endl; \

//ETX

#endif
