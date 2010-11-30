/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile$

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
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