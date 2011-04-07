#ifndef __vtkMedFilter_h_
#define __vtkMedFilter_h_

#include "vtkObject.h"
#include"vtkMed.h"

class VTK_EXPORT vtkMedFilter : public vtkObject
{
public:
  static vtkMedFilter* New();
  vtkTypeRevisionMacro(vtkMedFilter, vtkObject)
  void PrintSelf(ostream& os, vtkIndent indent);

protected :
	vtkMedFilter();
	~vtkMedFilter();

	med_filter Filter;
};

#endif //__vtkMedFilter_h_
