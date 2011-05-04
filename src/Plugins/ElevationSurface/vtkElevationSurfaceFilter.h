#ifndef _vtkElevationSurfaceFilter_h
#define _vtkElevationSurfaceFilter_h

#include "vtkAlgorithm.h"

class vtkPolyData;

class VTK_EXPORT vtkElevationSurfaceFilter: public vtkAlgorithm
{
public:
  static vtkElevationSurfaceFilter *New();
  vtkTypeRevisionMacro(vtkElevationSurfaceFilter,vtkAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // This is the factor applied to shrink the cell before extracting
  // the Elevation points.
  // A value of 0 shrinks the cells to their center, and a value of 1
  // do not shrink the cell at all.
  // default value 0.5
  vtkSetMacro(ScaleFactor,double);
  vtkGetMacro(ScaleFactor,double);

  // Description:
  // Set the direction of the deformation
  vtkSetVector3Macro(Direction, double);
  vtkGetVector3Macro(Direction, double);

  // Description:
  // If this flag is != 0, the filter will attempt detect automatically the
  // direction of the normal of the input.
  // The direction is given by the sum of the normals of all input polygons.
  // If there is no input poly, or if the sum is 0, it will revert to the
  // Direction ivar
  vtkBooleanMacro(AutoDetectDirection, int);
  vtkSetMacro(AutoDetectDirection, int);
  vtkGetMacro(AutoDetectDirection, int);

protected:
  vtkElevationSurfaceFilter();
  ~vtkElevationSurfaceFilter();

  int FillInputPortInformation(int , vtkInformation* );
  int FillOutputPortInformation(int , vtkInformation* );

  int ProcessRequest(vtkInformation* ,
                      vtkInformationVector** ,
                      vtkInformationVector* );


  int RequestData(vtkInformation *,
                  vtkInformationVector **,
                  vtkInformationVector *);

  int RequestInformation(vtkInformation *,
                         vtkInformationVector **,
                         vtkInformationVector *);

  int RequestUpdateExtent(vtkInformation* ,
                          vtkInformationVector**,
                          vtkInformationVector*);

  virtual void  ComputeDirection(vtkPolyData*, double *outDir);

  double ScaleFactor;
  double Direction[3];
  int AutoDetectDirection;

private:
  vtkElevationSurfaceFilter(const vtkElevationSurfaceFilter&);
  void operator =(const vtkElevationSurfaceFilter&);
};
#endif
