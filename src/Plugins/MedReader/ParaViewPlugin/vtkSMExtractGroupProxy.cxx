#include "vtkSMExtractGroupProxy.h"

#include "vtkObjectFactory.h"
#include "vtkSMMedHelper.h"

vtkStandardNewMacro(vtkSMExtractGroupProxy)
vtkCxxRevisionMacro(vtkSMExtractGroupProxy, "$Revision$")

vtkSMExtractGroupProxy::vtkSMExtractGroupProxy()
{
  this->Helper=vtkSMMedHelper::New();
  this->Helper->SetProxy(this);
}

vtkSMExtractGroupProxy::~vtkSMExtractGroupProxy()
{
  this->Helper->Delete();
}

void vtkSMExtractGroupProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
}
