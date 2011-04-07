#ifndef __vtkMedCartesianGrid_h_
#define __vtkMedCartesianGrid_h_

#include "vtkMedRegularGrid.h"

class VTK_EXPORT vtkMedCartesianGrid : public vtkMedRegularGrid
{
public :
  static vtkMedCartesianGrid* New();
  vtkTypeRevisionMacro(vtkMedCartesianGrid, vtkMedRegularGrid);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void  LoadCoordinates();

protected:
	vtkMedCartesianGrid();
  virtual ~vtkMedCartesianGrid();

private:
	vtkMedCartesianGrid(const vtkMedCartesianGrid&); // Not implemented.
  void operator=(const vtkMedCartesianGrid&); // Not implemented.

};

#endif //__vtkMedCartesianGrid_h_
