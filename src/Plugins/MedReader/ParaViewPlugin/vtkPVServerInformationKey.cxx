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
#include "vtkPVServerInformationKey.h"

#include "vtkAlgorithm.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#include "vtkExecutive.h"
#include "vtkInformation.h"
#include "vtkInformationDoubleVectorKey.h"
#include "vtkObjectFactory.h"
#include "vtkClientServerStream.h"
#include "vtkInformationIterator.h"
#include "vtkDataObject.h"
#include "vtkDataSetAttributes.h"
#include "vtkAbstractArray.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkCompositeDataIterator.h"
#include "vtkSMInformationKeyDomainHelper.h"

//----------------------------------------------------------------------------
vtkStandardNewMacro(vtkPVServerInformationKey)
;
vtkCxxRevisionMacro(vtkPVServerInformationKey, "$Revision$")
;

//----------------------------------------------------------------------------
class vtkPVServerInformationKeyInternals
{
public:
  vtkClientServerStream Result;
};

//----------------------------------------------------------------------------
vtkPVServerInformationKey::vtkPVServerInformationKey()
{
  this->Internal = new vtkPVServerInformationKeyInternals;
}

//----------------------------------------------------------------------------
vtkPVServerInformationKey::~vtkPVServerInformationKey()
{
  delete this->Internal;
}

//----------------------------------------------------------------------------
const vtkClientServerStream& vtkPVServerInformationKey::GetInformationOnKeys(
    vtkDataObject* data)
{
  this->Internal->Result.Reset();
  vtkSMInformationKeyDomainHelper* helper = vtkSMInformationKeyDomainHelper::New();
  helper->CopyToStream(data, this->Internal->Result);
  helper->Delete();
  return this->Internal->Result;
}

//----------------------------------------------------------------------------
void vtkPVServerInformationKey::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

