#ifndef _vtkELNOMeshFilter_h
#define _vtkELNOMeshFilter_h

#include "vtkUnstructuredGridAlgorithm.h"

class VTK_EXPORT vtkELNOMeshFilter: public vtkUnstructuredGridAlgorithm
{
public:
  static vtkELNOMeshFilter *New();
  vtkTypeRevisionMacro(vtkELNOMeshFilter,vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

protected:
  vtkELNOMeshFilter();
  ~vtkELNOMeshFilter();

  int RequestData(vtkInformation *, vtkInformationVector **,
      vtkInformationVector *);

private:
  vtkELNOMeshFilter(const vtkELNOMeshFilter&);
  void operator =(const vtkELNOMeshFilter&);
};
#endif
