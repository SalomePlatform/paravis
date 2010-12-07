#include "vtkSMMedReaderProxy.h"

#include "vtkObjectFactory.h"
#include "vtkSMMedHelper.h"

vtkStandardNewMacro(vtkSMMedReaderProxy)
vtkCxxRevisionMacro(vtkSMMedReaderProxy, "$Revision$")

vtkSMMedReaderProxy::vtkSMMedReaderProxy()
{
  this->Helper=vtkSMMedHelper::New();
  this->Helper->SetProxy(this);
}

vtkSMMedReaderProxy::~vtkSMMedReaderProxy()
{
  this->Helper->Delete();
}

void vtkSMMedReaderProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
}
