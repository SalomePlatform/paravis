#ifndef __vtkMedGrid_h_
#define __vtkMedGrid_h_

#include "vtkObject.h"
#include "vtkMedSetGet.h"
#include "vtkMed.h"

class vtkMedString;

class VTK_EXPORT vtkMedGrid : public vtkObject
{
public :
  vtkTypeRevisionMacro(vtkMedGrid, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The dimension of the space this mesh lives in.
  virtual void	SetDimension(med_int);
  vtkGetMacro(Dimension, med_int);

	// Description:
	// The units of each component of this field
	vtkGetObjectVectorMacro(Unit, vtkMedString);
	vtkSetObjectVectorMacro(Unit, vtkMedString);

	// Description:
	// The name of each component of this field
	vtkGetObjectVectorMacro(ComponentName, vtkMedString);
	vtkSetObjectVectorMacro(ComponentName, vtkMedString);

	// Description:
	// returns the number of points. Each sub class has to implement this method.
	virtual med_int	GetNumberOfPoints() = 0;

  // Description:
  // Initialize the global Ids of the first element of each MedEntityArray
  virtual void  InitializeCellGlobalIds(){;}

protected:
	vtkMedGrid();
  virtual ~vtkMedGrid();

  med_int Dimension;
  //BTX
  vtkObjectVector<vtkMedString>* Unit;
  vtkObjectVector<vtkMedString>* ComponentName;
  //ETX

private:
	vtkMedGrid(const vtkMedGrid&); // Not implemented.
  void operator=(const vtkMedGrid&); // Not implemented.

};

#endif //__vtkMedGrid_h_
