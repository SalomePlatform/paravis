#ifndef __vtkSMMedReaderProxy_h__
#define __vtkSMMedReaderProxy_h__

#include "vtkSMSourceProxy.h"
class vtkSMMedHelper;

class vtkSMMedReaderProxy : public vtkSMSourceProxy
{
public :
  static vtkSMMedReaderProxy* New();
  vtkTypeRevisionMacro(vtkSMMedReaderProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The Helper object deals with the group selections
  vtkGetObjectMacro(Helper, vtkSMMedHelper);

protected :

  vtkSMMedHelper* Helper;

  vtkSMMedReaderProxy();
  ~vtkSMMedReaderProxy();
};

#endif
