// Instantiate superclass first to give the template a DLL interface.
#include "vtkMed.h"

#include "vtkDataArrayTemplate.txx"
VTK_DATA_ARRAY_TEMPLATE_INSTANTIATE(med_int);

#include "vtkArrayIteratorTemplate.txx"
VTK_ARRAY_ITERATOR_TEMPLATE_INSTANTIATE(med_int);

#define __vtkMedIntArrayInternal_cxx
#include "vtkMedIntArrayInternal.h"

#include "vtkObjectFactory.h"

vtkCxxRevisionMacro(vtkMedIntArrayInternal, "$Revision$");
vtkStandardNewMacro(vtkMedIntArrayInternal);

//----------------------------------------------------------------------------
vtkMedIntArrayInternal::vtkMedIntArrayInternal(vtkIdType numComp): RealSuperclass(numComp)
{
}

//----------------------------------------------------------------------------
vtkMedIntArrayInternal::~vtkMedIntArrayInternal()
{
}

//----------------------------------------------------------------------------
void vtkMedIntArrayInternal::PrintSelf(ostream& os, vtkIndent indent)
{
  this->RealSuperclass::PrintSelf(os,indent);
}
