#ifndef __vtkSMExtractGroupProxy_h__
#define __vtkSMExtractGroupProxy_h__

#include "vtkSMSourceProxy.h"
class vtkSMMedHelper;

class vtkSMExtractGroupProxy : public vtkSMSourceProxy
{
public :
  static vtkSMExtractGroupProxy* New();
  vtkTypeRevisionMacro(vtkSMExtractGroupProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description:
  // The Helper object deals with the group selections
  vtkGetObjectMacro(Helper, vtkSMMedHelper);

protected :

  vtkSMMedHelper* Helper;

  vtkSMExtractGroupProxy();
  ~vtkSMExtractGroupProxy();
private :
  vtkSMExtractGroupProxy(const vtkSMExtractGroupProxy&);
  void  operator=(const vtkSMExtractGroupProxy&);
};

#endif //__vtkSMExtractGroupProxy_h__
