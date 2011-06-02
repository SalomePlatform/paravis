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

#include "vtkMedStandardGrid.h"

#include "vtkObjectFactory.h"
#include "vtkDataArray.h"

vtkCxxSetObjectMacro(vtkMedStandardGrid,Coordinates,vtkDataArray)
;

vtkCxxRevisionMacro(vtkMedStandardGrid, "$Revision$")
vtkStandardNewMacro(vtkMedStandardGrid)

vtkMedStandardGrid::vtkMedStandardGrid()
{
	this->Coordinates = NULL;
	this->CoordinateSystem = MED_CART;
	this->Size = NULL;
	this->NumberOfPoints = 0;
}

vtkMedStandardGrid::~vtkMedStandardGrid()
{
	this->SetCoordinates(NULL);
	if (this->Size != NULL)
		delete[] this->Size;
	this->Size = NULL;
}

void vtkMedStandardGrid::SetDimension(med_int dim)
{
	if (this->Size != NULL)
		delete[] this->Size;
	this->Size = NULL;
	if (dim > 0)
		{
		this->Size = new med_int[dim];
		}
	this->Superclass::SetDimension(dim);
}

void vtkMedStandardGrid::SetSize(med_int* size)
{
	if (this->Size == NULL || size == NULL)
		return;
	for (int dim = 0; dim < this->Dimension; dim++)
		{
		this->Size[dim] = size[dim];
		}
}

med_int* vtkMedStandardGrid::GetSize()
{
	return this->Size;
}

med_int vtkMedStandardGrid::GetSize(int dim)
{
	if (this->Size == NULL)
		return 0;
	return this->Size[dim];
}

void vtkMedStandardGrid::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
