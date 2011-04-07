#ifndef __vtkMedLink_h_
#define __vtkMedLink_h_

#include "vtkObject.h"
#include "vtkMed.h"

#include <string>
#include <map>

class vtkMedString;

class VTK_EXPORT vtkMedLink: public vtkObject
{
public:
  static vtkMedLink* New();
  vtkTypeRevisionMacro(vtkMedLink, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // the index of this field in the med file.
  vtkSetMacro(MedIterator, med_int);
  vtkGetMacro(MedIterator, med_int);

  // Description:
  // Set the name of the mesh linked to.
  vtkGetObjectMacro(MeshName, vtkMedString);

  // Description:
  // Set the name of the file this link points to.
  vtkSetStringMacro(Link);
  vtkGetStringMacro(Link);

  // Description:
  // returns the full path to the linked file.
  // If the Link is already a full path, it is returned.
  // If the directory is a relative path, the returned path is the
  // concatenation of the directory where the
  // original file is in and the Link.
  const char* GetFullLink(const char* originalFileName);

  // Description:
  // this stores the iterator that should be used when unmounting this link
  void  SetMountedIterator(med_class, med_int);
  med_int GetMountedIterator(med_class);

protected:
  vtkMedLink();
  virtual ~vtkMedLink();

  med_int MedIterator;
  vtkMedString* MeshName;
  char * Link;

  std::string FullLinkPath;

  // BTX
  std::map<med_class, int> Status;
  std::map<med_class, med_int> MountedIterator;
  //ETX

private:
  vtkMedLink(const vtkMedLink&); // Not implemented.
  void operator=(const vtkMedLink&); // Not implemented.

};

#endif //__vtkMedLink_h_
