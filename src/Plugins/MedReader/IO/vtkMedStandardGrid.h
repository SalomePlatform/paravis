// Copyright (C) 2010-2011  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
