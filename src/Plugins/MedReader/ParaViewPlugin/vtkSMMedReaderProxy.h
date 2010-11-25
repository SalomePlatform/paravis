#ifndef __vtkSMMedReaderProxy_h__
#define __vtkSMMedReaderProxy_h__

#include "vtkSMSourceProxy.h"
class vtkSMMedReaderProxyInternal;
class vtkSMSILModel;

class vtkSMMedReaderProxy : public vtkSMSourceProxy
{
public :
  static vtkSMMedReaderProxy* New();
  vtkTypeRevisionMacro(vtkSMMedReaderProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // Add all families of the given group to the selected families
  void SetGroupStatus(const char* key, int status);

  // Description :
  // Initialize the selections so that all cell groups are selected, but not the point groups.
  // All cell types are also selected.
  void  InitializeSelections();

  // Description :
  // find the child of a given vertex with the given name.
  // returns -1 on failure
  static vtkIdType FindChild(const char*, vtkIdType, vtkSMSILModel*);

protected :

  //BTX
  friend class vtkMedReaderProxyCommand;
  //ETX
  void  updateSIL();

  vtkSMMedReaderProxyInternal* Internal;

  vtkSMMedReaderProxy();
  ~vtkSMMedReaderProxy();
};

#endif
