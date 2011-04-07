#ifndef __vtkMedPolarGrid_h_
#define __vtkMedPolarGrid_h_

#include "vtkMedRegularGrid.h"

class VTK_EXPORT vtkMedPolarGrid : public vtkMedRegularGrid
{
public :
  static vtkMedPolarGrid* New();
  vtkTypeRevisionMacro(vtkMedPolarGrid, vtkMedRegularGrid);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual void  LoadCoordinates();

protected:
	vtkMedPolarGrid();
  virtual ~vtkMedPolarGrid();

private:
	vtkMedPolarGrid(const vtkMedPolarGrid&); // Not implemented.
  void operator=(const vtkMedPolarGrid&); // Not implemented.

};

#endif //__vtkMedPolarGrid_h_
