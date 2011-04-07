#ifndef __vtkMedCurvilinearGrid_h_
#define __vtkMedCurvilinearGrid_h_

#include "vtkMedGrid.h"
#include "vtkMed.h"

class vtkDataArray;
#include <vector>

class VTK_EXPORT vtkMedCurvilinearGrid : public vtkMedGrid
{
public :
  static vtkMedCurvilinearGrid* New();
  vtkTypeRevisionMacro(vtkMedCurvilinearGrid, vtkMedGrid)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // overloaded to allocate memory for the grid size
  virtual void	SetDimension(int);
  virtual int GetDimension();

  // Description:
  // The number of nodes of this grid. This method do not
  // perform any allocation.
  vtkSetMacro(NumberOfPoints, med_int);
  vtkGetMacro(NumberOfPoints, med_int);

  // Description:
  // this array contains the coordinates of the points used by this grid.
  virtual void	SetCoordinates(vtkDataArray*);
  vtkGetObjectMacro(Coordinates, vtkDataArray);

  // Description:
  // Set/Get the size of each axis.
  // Note that the number of points must match the product of the
  // size of each axis.
  virtual void  SetAxisSize(int axis, med_int size);
  virtual med_int GetAxisSize(int axis);

  virtual void  LoadCoordinates();

protected:
  vtkMedCurvilinearGrid();
  virtual ~vtkMedCurvilinearGrid();

  std::vector<med_int> AxisSize;
  med_int NumberOfPoints;
  vtkDataArray* Coordinates;

private:
  vtkMedCurvilinearGrid(const vtkMedCurvilinearGrid&); // Not implemented.
  void operator=(const vtkMedCurvilinearGrid&); // Not implemented.

};

#endif //__vtkMedCurvilinearGrid_h_
