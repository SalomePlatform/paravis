#ifndef __vtkMedFamily_h_
#define __vtkMedFamily_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkMedMesh;
class vtkMedGroup;
class vtkMedString;

class VTK_EXPORT vtkMedFamily: public vtkObject
{
public:
  static vtkMedFamily* New();
  vtkTypeRevisionMacro(vtkMedFamily, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Set the name of this family.
  vtkGetObjectMacro(Name, vtkMedString);

  // Description:
  // The id of this family.
  vtkGetMacro(Id, med_int);
  vtkSetMacro(Id, med_int);

  // Description:
  // Each family can be part of one or several groups.
  // This give access to the group names.
  vtkGetObjectVectorMacro(Group, vtkMedGroup);
  vtkSetObjectVectorMacro(Group, vtkMedGroup);

  // Description:
  // returns id this is a node or a cell centered family.
  vtkSetMacro(PointOrCell, int);
  vtkGetMacro(PointOrCell, int);

  // Description:
  // the index of this field in the med file.
  vtkSetMacro(MedIterator, med_int);
  vtkGetMacro(MedIterator, med_int);

protected:
  vtkMedFamily();
  virtual ~vtkMedFamily();

  med_int Id;
  med_int MedIterator;
  vtkMedString* Name;
  int PointOrCell;

  //BTX
  vtkObjectVector<vtkMedGroup>* Group;
  //ETX

private:
  vtkMedFamily(const vtkMedFamily&); // Not implemented.
  void operator=(const vtkMedFamily&); // Not implemented.

};

#endif //__vtkMedFamily_h_
