#ifndef _vtkELNOFilter_h
#define _vtkELNOFilter_h

#include "vtkQuadraturePointsGenerator.h"

class VTK_EXPORT vtkELNOFilter: public vtkQuadraturePointsGenerator
{
public:
  static vtkELNOFilter *New();
  vtkTypeRevisionMacro(vtkELNOFilter,vtkQuadraturePointsGenerator);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // This is the factor applied to shrink the cell before extracting
  // the ELNO points.
  // A value of 0 shrinks the cells to their center, and a value of 1
  // do not shrink the cell at all.
  // default value 0.5
  vtkSetMacro(ShrinkFactor,double);
  vtkGetMacro(ShrinkFactor,double);

protected:
  vtkELNOFilter();
  ~vtkELNOFilter();

  int RequestData(vtkInformation *, vtkInformationVector **,
      vtkInformationVector *);

  double ShrinkFactor;

private:
  vtkELNOFilter(const vtkELNOFilter&);
  void operator =(const vtkELNOFilter&);
};
#endif
