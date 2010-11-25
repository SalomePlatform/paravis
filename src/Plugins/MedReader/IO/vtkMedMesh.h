#ifndef __vtkMedMesh_h_
#define __vtkMedMesh_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkMedGrid;
class vtkMedFamily;
class vtkMedGroup;
class vtkMedString;
class vtkMedIntArray;
class vtkMedFamilyOnEntity;

class VTK_EXPORT vtkMedMesh: public vtkObject
{
public:
	static vtkMedMesh* New();
	vtkTypeRevisionMacro(vtkMedMesh, vtkObject);
	void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// The name of the mesh in the med file.
	vtkGetObjectMacro(Name, vtkMedString);

	// Description:
	// The universal name of the mesh.
	vtkGetObjectMacro(UniversalName, vtkMedString);

	// Description:
	// The name of the mesh in the med file.
	vtkGetObjectMacro(Description, vtkMedString);

	// Description:
	// the MedGrid is the object storing the actual data.
	virtual void SetGrid(vtkMedGrid*);
	vtkGetObjectMacro(Grid, vtkMedGrid);

	// Description:
	// Container of the families in this mesh
	vtkGetObjectVectorMacro(Family, vtkMedFamily);
	vtkSetObjectVectorMacro(Family, vtkMedFamily);
	virtual vtkMedFamily* GetOrCreateCellFamilyById(med_int);
	virtual int GetNumberOfCellFamily();

	// Description:
	// Get the Point Families
	vtkGetObjectVectorMacro(PointFamilyOnEntity, vtkMedFamilyOnEntity);
	vtkSetObjectVectorMacro(PointFamilyOnEntity, vtkMedFamilyOnEntity);
	virtual vtkMedFamily* GetOrCreatePointFamilyById(med_int);
	virtual vtkMedFamilyOnEntity* GetPointFamilyOnEntityById(med_int id);

	// Description:
	// Container of the groups in this mesh
	vtkGetObjectVectorMacro(PointGroup, vtkMedGroup);
	vtkSetObjectVectorMacro(PointGroup, vtkMedGroup);
  vtkGetObjectVectorMacro(CellGroup, vtkMedGroup);
  vtkSetObjectVectorMacro(CellGroup, vtkMedGroup);
	virtual vtkMedGroup*	GetGroup(int pointOrCell, const char*);

  // Description:
  // this array contains the global ids of the points used by the grid.
  virtual void	SetPointGlobalIds(vtkMedIntArray*);
  vtkGetObjectMacro(PointGlobalIds, vtkMedIntArray);

  // Description:
  // this array contains the family ids of the points used by the grid.
  virtual void	SetPointFamilyIds(vtkMedIntArray*);
  vtkGetObjectMacro(PointFamilyIds, vtkMedIntArray);

	// Description:
	// returns 1 if the global Ids array is set, and the number of tuples matches the number of points
	virtual int IsPointGlobalIdsLoaded();

	// Description:
	// returns 1 if the global Ids array is set, and the number of tuples matches the number of points
	virtual int IsPointFamilyIdsLoaded();

	// Description:
	// this id is the id to use when reading the med file
	vtkSetMacro(MedIndex, med_int);
	vtkGetMacro(MedIndex, med_int);

	// Description:
	// Gather the families that are present on this mesh nodes
	virtual void	ComputePointFamilies();

	// Description:
	// returns true if there is any node of this family on this mesh
	// you must have called ComputePointFamilies before.
	virtual int HasPointFamily(vtkMedFamily*);

protected:
	vtkMedMesh();
	virtual ~vtkMedMesh();

	vtkMedGrid* Grid;
	vtkMedString* Name;
	vtkMedString* UniversalName;
	vtkMedString* Description;
  vtkMedIntArray* PointGlobalIds;
  vtkMedIntArray* PointFamilyIds;
  med_int MedIndex;

	//BTX
	vtkObjectVector<vtkMedFamily>* Family;
	vtkObjectVector<vtkMedGroup>* PointGroup;
  vtkObjectVector<vtkMedGroup>* CellGroup;
	vtkObjectVector<vtkMedFamilyOnEntity>* PointFamilyOnEntity;
	//ETX

private:
	vtkMedMesh(const vtkMedMesh&); // Not implemented.
	void operator=(const vtkMedMesh&); // Not implemented.

};

#endif //__vtkMedMesh_h_
