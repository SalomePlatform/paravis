#ifndef __vtkMedGroup_h_
#define __vtkMedGroup_h_

#include "vtkObject.h"

class vtkMedString;

class VTK_EXPORT vtkMedGroup : public vtkObject
{
public :
  static vtkMedGroup* New();
  vtkTypeRevisionMacro(vtkMedGroup, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // This is the description of this file as stored in the med file.
  vtkGetStringMacro(Name);
  vtkSetStringMacro(Name);

	// Description:
	// This ivar says if this group contains point or cell families
	// This value is either vtkMedUtilities::OnPoint or vtkMedUtilities::OnCell
//	vtkSetMacro(PointOrCell, int);
//	vtkGetMacro(PointOrCell, int);

protected:
	vtkMedGroup();
  virtual ~vtkMedGroup();

  char* Name;

private:
	vtkMedGroup(const vtkMedGroup&); // Not implemented.
  void operator=(const vtkMedGroup&); // Not implemented.

};

#endif //__vtkMedGroup_h_
