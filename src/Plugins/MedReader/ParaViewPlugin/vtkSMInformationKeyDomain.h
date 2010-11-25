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
// .NAME vtkSMInformationKeyDomain - represents the possible values a property can have
// .SECTION Description
// vtkSMInformationKeyDomain is an abstract class that describes the "domain" of a
// a widget. A domain is a collection of possible values a property
// can have.
// Each domain can depend on one or more properties to compute it's
// values. This are called "required" properties and can be set in
// the XML configuration file.
// .SECTION See Also
// vtkSMProxyGroupDomain

#ifndef __vtkSMInformationKeyDomain_h
#define __vtkSMInformationKeyDomain_h

#include "vtkSMArrayListDomain.h"

class vtkSMProperty;
class vtkSMProxyLocator;
class vtkPVXMLElement;
class vtkSMInformationKeyDomainHelper;

class VTK_EXPORT vtkSMInformationKeyDomain : public vtkSMArrayListDomain
{
public:
  static vtkSMInformationKeyDomain* New();
  vtkTypeRevisionMacro(vtkSMInformationKeyDomain, vtkSMArrayListDomain);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // Returns true if the string is in the domain.
  virtual int IsInDomain(const char* string, unsigned int& idx);

  //BTX
  enum KeyDomainStrategy
  {
    NeedKey = 0, RejectKey = 1
  };
  //ETX

  // Description :
  // set the strategy of this Domain.
  // If set to NeedKey(0), only the fields with the given key will be in domain.
  // If set to RejectKey(1), only the fields that do not have the key will be in domain.
  // defaults to NeedKey (0)
  vtkSetMacro(KeyStrategy, int);
  vtkGetMacro(KeyStrategy, int);

  // Description:
  // This is the name of the vtk class in which the given key is defined
  vtkSetStringMacro(KeyLocation);
  vtkGetStringMacro(KeyLocation);

  // Description:
  // This is the name of the key
  vtkSetStringMacro(KeyName);
  vtkGetStringMacro(KeyName);

protected:
  vtkSMInformationKeyDomain();
  ~vtkSMInformationKeyDomain();

  // Description:
  // Set the appropriate ivars from the xml element. Should
  // be overwritten by subclass if adding ivars.
  virtual int ReadXMLAttributes(vtkSMProperty* prop, vtkPVXMLElement* elem);

  virtual void Update(vtkSMSourceProxy* sp, vtkSMInputArrayDomain* iad, int outputport);

  char* KeyLocation;
  char* KeyName;
  int KeyStrategy;
  vtkSMInformationKeyDomainHelper* Helper;

private:
  vtkSMInformationKeyDomain(const vtkSMInformationKeyDomain&); // Not implemented
  void operator=(const vtkSMInformationKeyDomain&); // Not implemented
};

#endif
