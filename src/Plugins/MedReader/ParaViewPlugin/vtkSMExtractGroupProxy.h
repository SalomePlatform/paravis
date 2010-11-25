#ifndef __vtkSMExtractGroupProxy_h__
#define __vtkSMExtractGroupProxy_h__

#include "vtkSMSourceProxy.h"
class vtkSMExtractGroupProxyInternal;
class vtkExtractGroupProxyCommand;

class vtkSMExtractGroupProxy : public vtkSMSourceProxy
{
public :
  static vtkSMExtractGroupProxy* New();
  vtkTypeRevisionMacro(vtkSMExtractGroupProxy, vtkSMSourceProxy);
  void PrintSelf(ostream& os, vtkIndent indent);

  // Description :
  // Add all families of the given group to the selected families
  virtual void SetGroupStatus(const char* key, int status);

protected :

  void  updateSIL();

  //BTX
  friend class vtkSMExtractGroupProxyInternal;
  friend class vtkExtractGroupProxyCommand;
  vtkSMExtractGroupProxyInternal* Internal;
  //ETX

  vtkSMExtractGroupProxy();
  ~vtkSMExtractGroupProxy();
private :
  vtkSMExtractGroupProxy(const vtkSMExtractGroupProxy&);
  void  operator=(const vtkSMExtractGroupProxy&);
};

#endif //__vtkSMExtractGroupProxy_h__
