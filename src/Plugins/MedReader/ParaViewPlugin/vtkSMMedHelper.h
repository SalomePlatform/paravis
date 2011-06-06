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

#ifndef __vtkSMMedHelper_h__
#define __vtkSMMedHelper_h__

#include "vtkObject.h"
class vtkSMMedHelperInternal;
class vtkSMProxy;

class vtkSMMedHelper: public vtkObject
{
public:
  static vtkSMMedHelper* New();
  vtkTypeRevisionMacro(vtkSMMedHelper, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // Initialize the selections so that all cell groups are selected,
  // but not the point groups.
  // All cell types are also selected.
  void InitializeSelections();

  // Description:
  // Set the proxy for which this helper will help deal with the
  // group selection
  void SetProxy(vtkSMProxy* proxy);

protected:

  //BTX
  friend class vtkMedHelperInternal;
  friend class vtkMedHelperCommand;
  vtkSMMedHelperInternal* Internal;
  //ETX

  void updateSIL();

  vtkSMMedHelper();
  ~vtkSMMedHelper();
};

#endif
