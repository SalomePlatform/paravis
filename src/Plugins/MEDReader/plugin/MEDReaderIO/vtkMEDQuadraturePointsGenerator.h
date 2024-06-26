// Copyright (C) 2010-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
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
// Author : Roman NIKOLAEV

#ifndef vtkMEDQuadraturePointsGenerator_h
#define vtkMEDQuadraturePointsGenerator_h

#include <vtkFiltersGeneralModule.h> // For export macro
#include <vtkQuadraturePointsGenerator.h>

class vtkInformation;
class vtkInformationVector;

class VTK_EXPORT vtkMEDQuadraturePointsGenerator : public vtkQuadraturePointsGenerator
{
public:
  vtkTypeMacro(vtkMEDQuadraturePointsGenerator,vtkQuadraturePointsGenerator)
  static vtkMEDQuadraturePointsGenerator *New();

protected:

  int RequestData(vtkInformation *req, vtkInformationVector **input, vtkInformationVector *output);
  vtkMEDQuadraturePointsGenerator();
  virtual ~vtkMEDQuadraturePointsGenerator();
private:
  vtkMEDQuadraturePointsGenerator(const vtkMEDQuadraturePointsGenerator &); // Not implemented
  void operator=(const vtkMEDQuadraturePointsGenerator &); // Not implemented
};

#endif
