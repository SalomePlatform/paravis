#ifndef _vtkScaleVectorFilter_h
#define _vtkScaleVectorFilter_h

#include "vtkPassInputTypeAlgorithm.h"

class VTK_EXPORT vtkScaleVectorFilter: public vtkPassInputTypeAlgorithm
{
public:
  static vtkScaleVectorFilter *New();
  vtkTypeRevisionMacro(vtkScaleVectorFilter,vtkPassInputTypeAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // This is the factor applied to the vector field.
  vtkSetClampMacro(ScaleFactor,double, -1, 1);
  vtkGetMacro(ScaleFactor,double);

protected:
  vtkScaleVectorFilter();
  ~vtkScaleVectorFilter();

  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *);

  int FillInputPortInformation(int port,
                               vtkInformation* info);

  double ScaleFactor;

private:
  vtkScaleVectorFilter(const vtkScaleVectorFilter&);
  void operator =(const vtkScaleVectorFilter&);
};
#endif
