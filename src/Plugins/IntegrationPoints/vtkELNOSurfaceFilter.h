#ifndef _vtkELNOSurfaceFilter_h
#define _vtkELNOSurfaceFilter_h

#include "vtkUnstructuredGridAlgorithm.h"

class VTK_EXPORT vtkELNOSurfaceFilter: public vtkUnstructuredGridAlgorithm
{
public:
  static vtkELNOSurfaceFilter *New();
  vtkTypeRevisionMacro(vtkELNOSurfaceFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkELNOSurfaceFilter();
  ~vtkELNOSurfaceFilter();

  int RequestData(vtkInformation *, vtkInformationVector **,
      vtkInformationVector *);

private:
  vtkELNOSurfaceFilter(const vtkELNOSurfaceFilter&);
  void operator =(const vtkELNOSurfaceFilter&);
};
#endif
