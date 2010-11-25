#ifndef __vtkMedUnstructuredGrid_h_
#define __vtkMedUnstructuredGrid_h_

#include "vtkMedGrid.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkDataArray;
class vtkMedEntityArray;
class vtkMedString;

class VTK_EXPORT vtkMedUnstructuredGrid : public vtkMedGrid
{
public :
  static vtkMedUnstructuredGrid* New();
  vtkTypeRevisionMacro(vtkMedUnstructuredGrid, vtkMedGrid)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // this array contains the coordinates of the points used by this grid.
  virtual void	SetCoordinates(vtkDataArray*);
  vtkGetObjectMacro(Coordinates, vtkDataArray);

  // Description:
  // Set the number of points in the grid.
  // no allocation is performed at this points, you have to allocate the Points
  // array manually.
  vtkSetMacro(NumberOfPoints, med_int);
  vtkGetMacro(NumberOfPoints, med_int);

  // Description:
  // This is the coordinate system the grid live in.
  vtkSetMacro(CoordinateSystem, med_repere);
  vtkGetMacro(CoordinateSystem, med_repere);

  // Description:
  // Add a cell array to this unstructured grid.
  // each cell array represents a different cell type.
	vtkGetObjectVectorMacro(EntityArray, vtkMedEntityArray);
	vtkSetObjectVectorMacro(EntityArray, vtkMedEntityArray);

	// Description:
	// returns true if the Coordinate array is set and corresponds
	// to the number of points
	virtual int IsCoordinatesLoaded();

	// Description:
	// return the vtkMedEntityArray that match the Entity type, if any.
	virtual vtkMedEntityArray* GetEntityArray(med_entite_maillage, med_geometrie_element);

  // Description:
  // Initialize the global Ids of the first element of each MedEntityArray
	virtual void  InitializeCellGlobalIds();

protected:
	vtkMedUnstructuredGrid();
  virtual ~vtkMedUnstructuredGrid();

  vtkDataArray* Coordinates;
  med_int NumberOfPoints;
  med_repere CoordinateSystem;
  //BTX
  vtkObjectVector<vtkMedEntityArray>* EntityArray;
	//ETX

private:
	vtkMedUnstructuredGrid(const vtkMedUnstructuredGrid&); // Not implemented.
  void operator=(const vtkMedUnstructuredGrid&); // Not implemented.
};

#endif //__vtkMedUnstructuredGrid_h_
