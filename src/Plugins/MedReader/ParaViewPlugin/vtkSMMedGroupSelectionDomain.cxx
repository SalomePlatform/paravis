#include "vtkSMMedGroupSelectionDomain.h"

#include "vtkObjectFactory.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkStringList.h"

#include <string>
using std::string;

vtkStandardNewMacro(vtkSMMedGroupSelectionDomain);

vtkSMMedGroupSelectionDomain::vtkSMMedGroupSelectionDomain()
{
}

vtkSMMedGroupSelectionDomain::~vtkSMMedGroupSelectionDomain()
{
}

int vtkSMMedGroupSelectionDomain::SetDefaultValues(vtkSMProperty* prop)
{
  vtkSMStringVectorProperty* svp = vtkSMStringVectorProperty::SafeDownCast(prop);
  if(!svp || this->GetNumberOfRequiredProperties() == 0)
    {
    return this->Superclass::SetDefaultValues(prop);
    }

  // info property has default values
  vtkSMStringVectorProperty* isvp = vtkSMStringVectorProperty::SafeDownCast(
    prop->GetInformationProperty());
  if (isvp)
    {
    vtkStringList* proplist = vtkStringList::New();
    svp->SetNumberOfElements(0);
    svp->SetNumberOfElementsPerCommand(2);
    for(int id=0; id<isvp->GetNumberOfElements(); id++)
      {
      string elem = isvp->GetElement(id);
      proplist->AddString(elem.c_str());
      if(elem.find("/OnCell/") != string::npos)
        {
        proplist->AddString("1");
        }
      else
        {
        proplist->AddString("0");
        }
      }
    svp->SetElements(proplist);
    proplist->Delete();
    }

  return 1;
}

void vtkSMMedGroupSelectionDomain::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
