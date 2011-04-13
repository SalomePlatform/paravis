#ifndef __vtkMedProfile_h_
#define __vtkMedProfile_h_

#include "vtkObject.h"
#include "vtkMed.h"

class vtkMedIntArray;
class vtkMedString;
class vtkMedFile;

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
  vtkGetStringMacro(Name);
  vtkSetStringMacro(Name);

  // Description:
  // Allocate and clear memory of this profil.
  virtual void	SetIds(vtkMedIntArray*);
  vtkGetObjectMacro(Ids, vtkMedIntArray);

  // Description:
  // return true if the index array is not null and the size match
  virtual int	IsLoaded();

	// Description:
	// The index of this field in the med file
	vtkSetMacro(MedIterator, med_int);
	vtkGetMacro(MedIterator, med_int);

	// Description:
	// Load the profile ids
	void	Load();

  // Description:
  // this is the file where this Profile is stored
  virtual void	SetParentFile(vtkMedFile*);
  vtkGetObjectMacro(ParentFile, vtkMedFile);

protected:
	vtkMedProfile();
  virtual ~vtkMedProfile();

  med_int MedIterator;
  vtkIdType NumberOfElement;
  vtkMedIntArray* Ids;
  char* Name;
  med_geometry_type GeometryType;
  vtkMedFile* ParentFile;

private:
	vtkMedProfile(const vtkMedProfile&); // Not implemented.
  void operator=(const vtkMedProfile&); // Not implemented.

};

#endif //__vtkMedProfil_h_
