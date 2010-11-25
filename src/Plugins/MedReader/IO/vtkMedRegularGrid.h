#ifndef __vtkMedRegularGrid_h_
#define __vtkMedRegularGrid_h_

#include "vtkMedGrid.h"
#include "vtkMed.h"
#include "vtkMedSetGet.h"

class vtkDataArray;

class VTK_EXPORT vtkMedRegularGrid : public vtkMedGrid
{
public :
  static vtkMedRegularGrid* New();
  vtkTypeRevisionMacro(vtkMedRegularGrid, vtkMedGrid)
  void PrintSelf(ostream& os, vtkIndent indent);

	// Description:
	// Container of the families in this mesh
	vtkGetObjectVectorMacro(Coordinates, vtkDataArray);
	vtkSetAbstractObjectVectorMacro(Coordinates, vtkDataArray);

	// Description:
	// overloaded to set the number of coordinates arrays. Do not allocate each array.
	virtual void	SetDimension(med_int);

  // Description:
  // the size of each dimension of the grid.
  virtual void	SetSize(med_int* size);
  virtual med_int* GetSize();
  virtual med_int GetSize(int dim);

  // Description:
  // returns the number of points of this grid.
  virtual med_int GetNumberOfPoints();

protected:
	vtkMedRegularGrid();
  virtual ~vtkMedRegularGrid();

  med_int *Size;
  //BTX
  vtkObjectVector<vtkDataArray>* Coordinates;
  //ETX

private:
	vtkMedRegularGrid(const vtkMedRegularGrid&); // Not implemented.
  void operator=(const vtkMedRegularGrid&); // Not implemented.

};

#endif //__vtkMedRegularGrid_h_
