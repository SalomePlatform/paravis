#ifndef __vtkMedRegularGrid_h_
#define __vtkMedRegularGrid_h_

#include "vtkMedGrid.h"
#include "vtkMed.h"
#include "vtkMedSetGet.h"

#include <vector>

class vtkDataArray;

class VTK_EXPORT vtkMedRegularGrid : public vtkMedGrid
{
public :
  static vtkMedRegularGrid* New();
  vtkTypeRevisionMacro(vtkMedRegularGrid, vtkMedGrid)
  void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Container of the families in this mesh
	vtkGetObjectVectorMacro(AxisCoordinate, vtkDataArray);
	vtkSetAbstractObjectVectorMacro(AxisCoordinate, vtkDataArray);

	// Description:
	// overloaded to set the number of coordinates arrays.
	// Do not allocate each array.
	virtual void	SetDimension(med_int);
	virtual int	GetDimension();

  // Description:
  // the size of each dimension of the grid.
  // SetDimension has to have been called before
  virtual void	SetAxisSize(int axis, med_int size);
  virtual med_int GetAxisSize(int dim);

  // Description:
  // returns the number of points of this grid.
  virtual med_int GetNumberOfPoints();

  virtual void  LoadCoordinates();

protected:
	vtkMedRegularGrid();
  virtual ~vtkMedRegularGrid();

  std::vector<med_int> AxisSize;
  //BTX
  vtkObjectVector<vtkDataArray>* AxisCoordinate;
  //ETX

private:
	vtkMedRegularGrid(const vtkMedRegularGrid&); // Not implemented.
  void operator=(const vtkMedRegularGrid&); // Not implemented.

};

#endif //__vtkMedRegularGrid_h_
