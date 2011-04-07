#ifndef __vtkMedSelection_h_
#define __vtkMedSelection_h_

#include "vtkObject.h"

class vtkMedSelectionInternals;

class VTK_EXPORT vtkMedSelection : public vtkObject
{
public :
  static vtkMedSelection* New();
  vtkTypeRevisionMacro(vtkMedSelection, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void  Initialize();

  virtual void  AddKey(const char*);

  virtual void  SetKeyStatus(const char* name, int status);

  virtual int GetKeyStatus(const char* name);

  virtual const char* GetKey(int index);

  virtual int GetNumberOfKey();

  virtual int KeyExists(const char* name);

  virtual void  RemoveKeyByIndex(int index);

protected:
  vtkMedSelection();
  virtual ~vtkMedSelection();

 vtkMedSelectionInternals* Internals;

private:
  vtkMedSelection(const vtkMedSelection&); // Not implemented.
  void operator=(const vtkMedSelection&); // Not implemented.

};

#endif //__vtkMedSelection_h_
