#ifndef __vtkMedProfile_h_
#define __vtkMedProfile_h_

#include "vtkObject.h"
#include "vtkMed.h"

class vtkMedIntArray;
class vtkMedString;

class VTK_EXPORT vtkMedProfile : public vtkObject
{
public :
  static vtkMedProfile* New();
  vtkTypeRevisionMacro(vtkMedProfile, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This is the number of elements of this profile
  vtkSetMacro(NumberOfElement, vtkIdType);
  vtkGetMacro(NumberOfElement, vtkIdType);

  // Description:
  // The name of the profile in the file
	vtkGetObjectMacro(Name, vtkMedString);

  // Description:
  // Allocate and clear memory of this profil.
  virtual void	SetIds(vtkMedIntArray*);
  vtkGetObjectMacro(Ids, vtkMedIntArray);

  // Description:
  // return true if the index array is not null and the size match
  virtual int	IsLoaded();

	// Description:
	// The index of this field in the med file
	vtkSetMacro(MedIndex, med_int);
	vtkGetMacro(MedIndex, med_int);

protected:
	vtkMedProfile();
  virtual ~vtkMedProfile();

  med_int MedIndex;
  vtkIdType NumberOfElement;
  vtkMedIntArray* Ids;
  vtkMedString* Name;

private:
	vtkMedProfile(const vtkMedProfile&); // Not implemented.
  void operator=(const vtkMedProfile&); // Not implemented.

};

#endif //__vtkMedProfil_h_
