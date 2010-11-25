#ifndef __vtkMedEntityArray_h_
#define __vtkMedEntityArray_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkMedIntArray;
class vtkFamilyIdSet;
class vtkMedFamily;
class vtkMedFamilyOnEntity;
class vtkMedMesh;

class VTK_EXPORT vtkMedEntityArray: public vtkObject
{
public:
  static vtkMedEntityArray* New();
  vtkTypeRevisionMacro(vtkMedEntityArray, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This is the number of cells of this entity type.
  vtkSetMacro(NumberOfEntity, vtkIdType);
  vtkGetMacro(NumberOfEntity, vtkIdType);

  // Description:
  // the support of the cells : one of
  // MED_MAILLE, MED_FACE, MED_ARETE, MED_NOEUD, MED_NOEUD_MAILLE
  vtkSetMacro(Type, med_entite_maillage);
  vtkGetMacro(Type, med_entite_maillage);

  // Description:
  // The type of the cells : one of
  // MED_POINT1, MED_SEG2, MED_SEG3, MED_TRIA3, MED_QUAD4, MED_TRIA6, MED_QUAD8,
	// MED_TETRA4, MED_PYRA5, MED_PENTA6, MED_HEXA8, MED_TETRA10, MED_PYRA13,
	// MED_PENTA15, MED_HEXA20, MED_POLYGONE, MED_POLYEDRE
  vtkSetMacro(Geometry, med_geometrie_element);
  vtkGetMacro(Geometry, med_geometrie_element);

  // Description:
  // This connectivity type of this entity : one of
  // MED_NOD, MED_DESC
  vtkSetMacro(Connectivity, med_connectivite);
  vtkGetMacro(Connectivity, med_connectivite);

  // Description:
  // This array stores the family ids of each entity.
  virtual void	SetConnectivityArray(vtkMedIntArray*);
  vtkGetObjectMacro(ConnectivityArray, vtkMedIntArray);

  // Description:
  // This array stores the connectivity array for this entity.
  virtual void	SetFamilyIds(vtkMedIntArray*);
  vtkGetObjectMacro(FamilyIds, vtkMedIntArray);

  // Description:
  // This array stores the global Ids of the entities.
  virtual void	SetGlobalIds(vtkMedIntArray*);
  vtkGetObjectMacro(GlobalIds, vtkMedIntArray);

  // Description:
  // For polygons, this array stores the index of each edge described in the connectivity array
  // For polyhedrons, this arrays stores the index of each face described in the Index1 array
  virtual void	SetIndex0(vtkMedIntArray*);
  vtkGetObjectMacro(Index0, vtkMedIntArray);

  // Description:
  // For polyhedrons, this arrays can store either
  // the index of each node of each face described in the Index1 array (node connectivity)
  // or the type each face described in the Index1 array (hierarchical connectivity)
  virtual void	SetIndex1(vtkMedIntArray*);
  vtkGetObjectMacro(Index1, vtkMedIntArray);

  vtkSetObjectVectorMacro(FamilyOnEntity, vtkMedFamilyOnEntity);
  vtkGetObjectVectorMacro(FamilyOnEntity, vtkMedFamilyOnEntity);

  // Description:
  // Compute the list of families that are on this array
  virtual void	ComputeFamilies(vtkMedMesh*);

  // Description:
  // returns true if there are cells of the given family on this entity.
  virtual int	HasFamily(vtkMedFamily*);

  // Description:
  // returns 1 if the connectivity array is set and matches the number of connectivity elements.
  virtual int	IsConnectivityLoaded();

  // Description:
  // returns 1 if the family ids is set and matches the number of entities.
  virtual int	IsFamilyIdsLoaded();

  // Description:
  // returns 1 if the global ids is set and matches the number of entities.
  virtual int	IsGlobalIdsLoaded();

  // Description :
  // This gives the global id of the first element of this array.
  vtkSetMacro(InitialGlobalId, vtkIdType);
  vtkGetMacro(InitialGlobalId, vtkIdType);

protected:
  vtkMedEntityArray();
  virtual ~vtkMedEntityArray();

  vtkIdType NumberOfEntity;
  med_geometrie_element	Geometry;
  med_entite_maillage	Type;
  med_connectivite Connectivity;
  vtkIdType InitialGlobalId;

  vtkMedIntArray* FamilyIds;
  vtkMedIntArray* GlobalIds;
  vtkMedIntArray* ConnectivityArray;
  vtkMedIntArray* Index0; // used by polygons and polyhedrons
  vtkMedIntArray* Index1; // used by polygons and polyhedrons

  //BTX
	vtkObjectVector<vtkMedFamilyOnEntity>* FamilyOnEntity;
	//ETX

private:
  vtkMedEntityArray(const vtkMedEntityArray&); // Not implemented.
  void operator=(const vtkMedEntityArray&); // Not implemented.
};

#endif //__vtkMedEntityArray_h_
