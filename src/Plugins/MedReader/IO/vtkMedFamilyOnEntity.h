#ifndef __vtkMedFamilyOnEntity_h_
#define __vtkMedFamilyOnEntity_h_

// Description :
// This class represents the intersection between a family and an entity.

#include "vtkObject.h"
#include "vtkMed.h"
#include "vtkMedUtilities.h"

class vtkMedEntityArray;
class vtkMedFamily;
class vtkMedGrid;
class vtkMedFamilyOnEntityOnProfile;
class vtkMedIntArray;

class VTK_EXPORT vtkMedFamilyOnEntity : public vtkObject
{
public :
  static vtkMedFamilyOnEntity* New();
  vtkTypeRevisionMacro(vtkMedFamilyOnEntity, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This is the family of this family on entity.
  virtual void  SetFamily(vtkMedFamily*);
  vtkGetObjectMacro(Family, vtkMedFamily);

  // Description:
  // This is the entity array this family on entity is on.
  virtual void  SetEntityArray(vtkMedEntityArray*);
  vtkGetObjectMacro(EntityArray, vtkMedEntityArray);

  // Description:
  // This is the grid this family on entity is reffering to
  virtual void  SetParentGrid(vtkMedGrid*);
  vtkGetObjectMacro(ParentGrid, vtkMedGrid);

  // Description :
  //  Returns vtkMedUtilities::OnPoint (0) or vtkMedUtilities::OnCell (1)
  virtual int  GetPointOrCell();

  // Description :
  // Returns true if the family is on points or if the entity is MED_POINT
  virtual int  GetVertexOnly();

  // Description:
  // returns the entity descriptor associated with this family on entity
  virtual vtkMedEntity GetEntity();

  // Description:
  // Fields can use profile to be stored on a subset of entities.
  // a priori, profiles and families are independent notions.
  // In case there are profiles, we create the intersection of the profile and
  // the FamilyOnEntity to be able to map the field on the geometry.
  // BEFORE calling those function, you have to have assigned a profile
  // to the vtkMedFamilyOnEntityOnProfile
  void  AddFamilyOnEntityOnProfile(vtkMedFamilyOnEntityOnProfile*);
  int GetNumberOfFamilyOnEntityOnProfile();
  vtkMedFamilyOnEntityOnProfile* GetFamilyOnEntityOnProfile(vtkMedProfile*);
  vtkMedFamilyOnEntityOnProfile* GetFamilyOnEntityOnProfile(int index);

protected:
  vtkMedFamilyOnEntity();
  virtual ~vtkMedFamilyOnEntity();

  vtkMedFamily* Family;
  vtkMedEntityArray* EntityArray;
  vtkMedGrid* ParentGrid;

  std::map<vtkMedProfile*, vtkSmartPointer<vtkMedFamilyOnEntityOnProfile> >
      FamilyOnEntityOnProfile;

private:
  vtkMedFamilyOnEntity(const vtkMedFamilyOnEntity&); // Not implemented.
  void operator=(const vtkMedFamilyOnEntity&); // Not implemented.

};

#endif //__vtkMedFamilyOnEntity_h_
