#ifndef __vtkMedFamilyOnEntity_h_
#define __vtkMedFamilyOnEntity_h_

// Description :
// This class represents the intersection between a family and an entity.
// This is the smallest partition of the the support in a med file.

#include "vtkObject.h"
#include "vtkMed.h"

class vtkMedEntityArray;
class vtkMedFamily;
class vtkMedMesh;

class VTK_EXPORT vtkMedFamilyOnEntity : public vtkObject
{
public :
  static vtkMedFamilyOnEntity* New();
  vtkTypeRevisionMacro(vtkMedFamilyOnEntity, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void	SetFamily(vtkMedFamily*);
  vtkGetObjectMacro(Family, vtkMedFamily);

  virtual void	SetEntityArray(vtkMedEntityArray*);
  vtkGetObjectMacro(EntityArray, vtkMedEntityArray);

  virtual void  SetMesh(vtkMedMesh*);
  vtkGetObjectMacro(Mesh, vtkMedMesh);

  // Description :
  //  Returns vtkMedUtilities::OnPoint (0) or vtkMedUtilities::OnCell (1)
  virtual int  GetPointOrCell();

  // Description :
  // Returns true if the family is on points or if the entity is MED_POINT
  virtual int  GetVertexOnly();

  virtual med_entite_maillage	GetType();
  virtual med_geometrie_element	GetGeometry();

  // Description:
  // This flag says if this vtkMedFamilyOnEntity covers all the points of the mesh or not.
  // -1 means it has not been computed, 1/0 means it covers or not all the points.
  vtkSetMacro(AllPoints, int)
  vtkGetMacro(AllPoints, int)

protected:
	vtkMedFamilyOnEntity();
  virtual ~vtkMedFamilyOnEntity();

  vtkMedFamily* Family;
  vtkMedEntityArray* EntityArray;
  vtkMedMesh* Mesh;
  int AllPoints;

private:
	vtkMedFamilyOnEntity(const vtkMedFamilyOnEntity&); // Not implemented.
  void operator=(const vtkMedFamilyOnEntity&); // Not implemented.

};

#endif //__vtkMedFamilyOnEntity_h_
