#ifndef __vtkMedDriver23_h_
#define __vtkMedDriver23_h_

#include "vtkMedDriver.h"

class VTK_EXPORT vtkMedDriver23: public vtkMedDriver
{
public:
  static vtkMedDriver23* New();
  vtkTypeRevisionMacro(vtkMedDriver23, vtkMedDriver)
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkMedDriver23();
  virtual ~vtkMedDriver23();

private:
  vtkMedDriver23(const vtkMedDriver23&); // Not implemented.
  void operator=(const vtkMedDriver23&); // Not implemented.
};

#endif //__vtkMedDriver23_h_
