#ifndef __vtkMedFactory_h_
#define __vtkMedFactory_h_

#include "vtkObject.h"
class vtkMedDriver;

class VTK_EXPORT vtkMedFactory: public vtkObject
{
public:
  static vtkMedFactory* New();
  vtkTypeRevisionMacro(vtkMedFactory, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual vtkMedDriver* NewMedDriver(int major, int minor, int release);

protected:
  vtkMedFactory();
  virtual ~vtkMedFactory();

private:
  vtkMedFactory(const vtkMedFactory&); // Not implemented.
  void operator=(const vtkMedFactory&); // Not implemented.
};

#endif //__vtkMedFactory_h_
