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
// .NAME vtkSMInformationKeyDomainHelper - a helper used to send information key information
// from the server to the client
// .SECTION Description
// vtkSMInformationKeyDomainHelper can populate a stream object from a
// vtkDataObject with the contained keys of each field,
// and can be populated from the same stream on the clietn side
// to give those informations back
// .SECTION See Also
// vtkSMInformationKeyDomain vtkPVServerInformationKey

#ifndef __vtkSMInformationKeyDomainHelper_h
#define __vtkSMInformationKeyDomainHelper_h

#include "vtkObject.h"

class vtkDataObject;
class vtkClientServerStream;
class vtkSMInformationKeyDomainHelperInternal;

class VTK_EXPORT vtkSMInformationKeyDomainHelper: public vtkObject
{
public:
  static vtkSMInformationKeyDomainHelper* New();
  vtkTypeRevisionMacro(vtkSMInformationKeyDomainHelper, vtkObject);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // Initialize this object from the given stream.
  // This is supposed to be called on the client side.
  virtual int InitializeFromStream(const vtkClientServerStream& stream);

  // Description :
  // Create a stream object from the information keys present in a given
  // data object.
  virtual int CopyToStream(vtkDataObject* data, vtkClientServerStream& stream);

  virtual int GetNumberOfFields(int attributeType);
  virtual const char* GetFieldName(int attributeType, int fieldId);

  virtual int FieldHasKey(const char* field, const char* location, const char* name);

protected:
  vtkSMInformationKeyDomainHelper();
  ~vtkSMInformationKeyDomainHelper();

  // Description :
  // Append the keys present in the given DataObject to the
  // internal data structure.
  void AppendLeaf(vtkDataObject* data);

  vtkSMInformationKeyDomainHelperInternal* Internal;

private:
  vtkSMInformationKeyDomainHelper(const vtkSMInformationKeyDomainHelper&); // Not implemented
  void operator=(const vtkSMInformationKeyDomainHelper&); // Not implemented
};

#endif
