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
#include "vtkSMInformationKeyDomain.h"
#include "vtkClientServerStream.h"
#include "vtkProcessModule.h"
#include "vtkSMOutputPort.h"
#include "vtkPVXMLElement.h"
#include "vtkSMSourceProxy.h"
#include "vtkObjectFactory.h"
#include "vtkSMInformationKeyDomainHelper.h"

vtkCxxRevisionMacro(vtkSMInformationKeyDomain, "$Revision$");
vtkStandardNewMacro(vtkSMInformationKeyDomain);

//---------------------------------------------------------------------------
vtkSMInformationKeyDomain::vtkSMInformationKeyDomain()
{
  this->KeyLocation = NULL;
  this->KeyName = NULL;
  this->KeyStrategy = NeedKey;
  this->Helper = vtkSMInformationKeyDomainHelper::New();
}

//---------------------------------------------------------------------------
vtkSMInformationKeyDomain::~vtkSMInformationKeyDomain()
{
  this->SetKeyName(NULL);
  this->SetKeyLocation(NULL);
  this->Helper->Delete();
}

//---------------------------------------------------------------------------
int vtkSMInformationKeyDomain::ReadXMLAttributes(vtkSMProperty* prop,
    vtkPVXMLElement* elem)
{
  if(!this->Superclass::ReadXMLAttributes(prop, elem))
    return 0;

  const char* location = elem->GetAttribute("key_location");
  this->SetKeyLocation(location);
  const char* name = elem->GetAttribute("key_name");
  this->SetKeyName(name);
  const char* strategy = elem->GetAttribute("key_strategy");
  if(strategy && strcmp(strategy, "reject_key"))
    {
    this->SetKeyStrategy(RejectKey);
    }
  else if(strategy && strcmp(strategy, "need_key"))
    {
    this->SetKeyStrategy(NeedKey);
    }
  else
    {
    this->SetKeyStrategy(NeedKey);
    }
  return name != NULL && location != NULL;
}

//---------------------------------------------------------------------------
void vtkSMInformationKeyDomain::Update(vtkSMSourceProxy* sp,
    vtkSMInputArrayDomain* iad, int outputport)
{
  // Make sure the outputs are created.
  sp->CreateOutputPorts();
  vtkSMOutputPort* op = sp->GetOutputPort(outputport);
  vtkSMProxy* pxy = (op ? op->GetDataObjectProxy(1) : NULL);
  if(this->KeyLocation != NULL && this->KeyName != NULL && pxy)
    {
    vtkClientServerID objectId = pxy->GetID();

    vtkClientServerStream stream;
    vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
    vtkClientServerID serverObjID = pm->NewStreamObject(
        "vtkPVServerInformationKey", stream);
    stream << vtkClientServerStream::Invoke << serverObjID
        << "SetProcessModule" << pm->GetProcessModuleID()
        << vtkClientServerStream::End;
    stream << vtkClientServerStream::Invoke << serverObjID
        << "GetInformationOnKeys" << objectId
        << vtkClientServerStream::End;
    pm->SendStream(sp->GetConnectionID(), sp->GetServers(), stream);

    const vtkClientServerStream& result = pm->GetLastResult(
        sp->GetConnectionID(), sp->GetServers());

    vtkClientServerStream res;
    result.GetArgument(0, 0, &res);

    this->Helper->InitializeFromStream(res);
    this->RemoveAllStrings();

    int nf = this->Helper->GetNumberOfFields(this->AttributeType);
    for(int ff = 0; ff < nf; ff++)
      {
      const char* name = this->Helper->GetFieldName(this->AttributeType, ff);
      unsigned int idx;
      if(!this->Superclass::IsInDomain(name, idx))
        this->AddString(name);
      }
    }
}

int vtkSMInformationKeyDomain::IsInDomain(const char* string, unsigned int& idx)
{
  int hasKey = this->Helper->FieldHasKey(string, this->KeyLocation, this->KeyName);

  if(this->KeyStrategy == NeedKey)
    {
    return hasKey;
    }
  else
    {
    return !hasKey;
    }
}


//---------------------------------------------------------------------------
void vtkSMInformationKeyDomain::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
