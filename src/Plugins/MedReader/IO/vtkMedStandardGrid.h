#ifndef __vtkMedStandardGrid_h_
#define __vtkMedStandardGrid_h_

#include "vtkMedGrid.h"

class vtkDataArray;

class VTK_EXPORT vtkMedStandardGrid : public vtkMedGrid
{
public :
  static vtkMedStandardGrid* New();
  vtkTypeRevisionMacro(vtkMedStandardGrid, vtkMedGrid)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // overloaded to allocate memory for the grid size
  virtual void	SetDimension(med_int);

  // Description:
  // the coordinate system describes the system in which the coordinates are given
  vtkSetMacro(CoordinateSystem, med_repere);
  vtkGetMacro(CoordinateSystem, med_repere);

  // Description:
  // The number of nodes of this grid. This method do not perform any allocation.
  vtkSetMacro(NumberOfPoints, med_int);
  vtkGetMacro(NumberOfPoints, med_int);

  // Description:
  // this array contains the coordinates of the points used by this grid.
  virtual void	SetCoordinates(vtkDataArray*);
  vtkGetObjectMacro(Coordinates, vtkDataArray);

  // Description:
  // the size of each dimension of the grid.
  virtual void	SetSize(med_int* size);
  virtual med_int* GetSize();
  virtual med_int GetSize(int dim);

protected:
	vtkMedStandardGrid();
  virtual ~vtkMedStandardGrid();

  med_int *Size;
  med_repere CoordinateSystem;
  med_int NumberOfPoints;
  vtkDataArray* Coordinates;

private:
	vtkMedStandardGrid(const vtkMedStandardGrid&); // Not implemented.
  void operator=(const vtkMedStandardGrid&); // Not implemented.

};

#endif //__vtkMedStandardGrid_h_
