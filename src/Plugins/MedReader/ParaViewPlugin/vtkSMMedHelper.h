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
