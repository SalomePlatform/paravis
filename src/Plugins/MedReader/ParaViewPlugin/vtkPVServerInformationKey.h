/*=========================================================================

 Program:   ParaView
 Module:    $RCSfile$

 Copyright (c) Kitware, Inc.
 All rights reserved.
 See Copyright.txt or http://www.paraview.org/HTML/Copyright.html for details.

 This software is distributed WITHOUT ANY WARRANTY; without even
 the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 PURPOSE.  See the above copyright notice for more information.

 =========================================================================*/
// .NAME vtkPVServerInformationKey - Server-side helper for vtkSMInformationKeyDomain.
// .SECTION Description
// Get if the object has the given key in its information.

#ifndef __vtkPVServerInformationKey_h
#define __vtkPVServerInformationKey_h

#include "vtkPVServerObject.h"

class vtkClientServerStream;
class vtkDataObject;
class vtkPVServerInformationKeyInternals;

class VTK_EXPORT vtkPVServerInformationKey: public vtkPVServerObject
{
public:
  static vtkPVServerInformationKey* New();
vtkTypeRevisionMacro(vtkPVServerInformationKey, vtkPVServerObject)
  ;
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Gather all inforamtion keys present in all fields of this object.
  const vtkClientServerStream& GetInformationOnKeys(
      vtkDataObject* data);

protected:
  vtkPVServerInformationKey();
  ~vtkPVServerInformationKey();

  // Internal implementation details.
  vtkPVServerInformationKeyInternals* Internal;

private:
  vtkPVServerInformationKey(const vtkPVServerInformationKey&); // Not implemented
  void operator=(const vtkPVServerInformationKey&); // Not implemented
};

#endif
