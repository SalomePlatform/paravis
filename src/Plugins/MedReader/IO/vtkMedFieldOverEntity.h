#ifndef __vtkMedFieldOverEntity_h_
#define __vtkMedFieldOverEntity_h_

#include "vtkObject.h"
#include "vtkMed.h"

#include "vtkMedSetGet.h"
#include "vtkMedUtilities.h"

class vtkMedFieldOnProfile;
class vtkMedFieldStep;

class VTK_EXPORT vtkMedFieldOverEntity: public vtkObject
{
public:
  static vtkMedFieldOverEntity* New();
  vtkTypeRevisionMacro(vtkMedFieldOverEntity, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // the support of the cells : one of
  void  SetEntity(const vtkMedEntity& entity){this->Entity = entity;}
  const vtkMedEntity& GetEntity(){return this->Entity;}

	// Description:
	// This is the vtkMedFieldStep that owns this vtkMedFieldOverEntity
	virtual void	SetParentStep(vtkMedFieldStep*);
	vtkGetObjectMacro(ParentStep, vtkMedFieldStep);

  // Description:
  // This array store for each profile the field over this profile
  vtkGetObjectVectorMacro(FieldOnProfile, vtkMedFieldOnProfile);
  vtkSetObjectVectorMacro(FieldOnProfile, vtkMedFieldOnProfile);

  // Description:
  // This flag is set during the information pass, and tells if
  // there is a profile of not.
  // Note that if there is no profile, a dummy vtkMedFieldOnProfile
  // is created to store the actual data.
  vtkGetMacro(HasProfile, int);
  vtkSetMacro(HasProfile, int);

protected:
  vtkMedFieldOverEntity();
  virtual ~vtkMedFieldOverEntity();

  vtkMedFieldStep* ParentStep;

  vtkMedEntity	Entity;

  int HasProfile;

  //BTX
  vtkObjectVector<vtkMedFieldOnProfile>* FieldOnProfile;
  //ETX

private:
  vtkMedFieldOverEntity(const vtkMedFieldOverEntity&); // Not implemented.
  void operator=(const vtkMedFieldOverEntity&); // Not implemented.
};

#endif //__vtkMedFieldOverEntity_h_
