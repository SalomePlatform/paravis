// .NAME vtkSMMedGroupSelectionDompain - domain used to
// select groups from med files.
// .SECTION Description
// This property overrides the SetDefaultValues to select only cell groups

#ifndef __vtkSMMedGroupSelectionDomain_h
#define __vtkSMMedGroupSelectionDomain_h

#include "vtkSMSILDomain.h"

class VTK_EXPORT vtkSMMedGroupSelectionDomain : public vtkSMSILDomain
{
public:
  static vtkSMMedGroupSelectionDomain* New();
  vtkTypeMacro(vtkSMMedGroupSelectionDomain, vtkSMSILDomain);
  void PrintSelf(ostream& os, vtkIndent indent);

  virtual int SetDefaultValues(vtkSMProperty*);

protected:
  vtkSMMedGroupSelectionDomain();
  ~vtkSMMedGroupSelectionDomain();

private:
  vtkSMMedGroupSelectionDomain(const vtkSMMedGroupSelectionDomain&);
    // Not implemented
  void operator=(const vtkSMMedGroupSelectionDomain&); // Not implemented
};

#endif
