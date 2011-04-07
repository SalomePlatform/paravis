#include "vtkMedFilter.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMedFilter, "$Revision$")
vtkStandardNewMacro(vtkMedFilter)

vtkMedFilter::vtkMedFilter() : Filter((med_filter)MED_FILTER_INIT)
{
}

vtkMedFilter::~vtkMedFilter()
{
	MEDfilterClose(&this->Filter);
}

void vtkMedFilter::PrintSelf(ostream& os, vtkIndent indent)
{
	this->Superclass::PrintSelf(os, indent);
}
