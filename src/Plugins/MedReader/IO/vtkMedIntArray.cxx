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
#include "vtkMedIntArray.h"

#include "vtkObjectFactory.h"

//----------------------------------------------------------------------------
vtkCxxRevisionMacro(vtkMedIntArray, "$Revision$");
vtkStandardNewMacro(vtkMedIntArray);

//----------------------------------------------------------------------------
vtkMedIntArray::vtkMedIntArray(vtkIdType numComp): Superclass(numComp)
{
}

//----------------------------------------------------------------------------
vtkMedIntArray::~vtkMedIntArray()
{
}

//----------------------------------------------------------------------------
void vtkMedIntArray::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os,indent);
}
