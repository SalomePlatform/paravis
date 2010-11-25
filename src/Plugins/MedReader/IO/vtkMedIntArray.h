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
// .NAME vtkMedIntArray - dynamic, self-adjusting array of med_int
// .SECTION Description
// vtkMedIntArray is an array of values of type med_int.
// It provides methods for insertion and retrieval of values and will
// automatically resize itself to hold new data.
// If the med_int type is the same as the vtkIdType, this class inherits from the
// vtkIdType array, allowing safe shallow copies.

#ifndef __vtkMedIntArray_h
#define __vtkMedIntArray_h

#include "vtkMedUtilities.h"
#include "vtkMed.h"

#include "vtkMedIntArrayInternal.h"
#include "vtkIdTypeArray.h"
#include "vtkIntArray.h"

//BTX
template <class T1> struct med_int_vtkIdType_Traits : IsSameTraits<T1, med_int>
{
	typedef vtkMedIntArrayInternal Superclass;
};

template <> struct med_int_vtkIdType_Traits<med_int> : IsSameTraits<med_int, med_int>
{
	typedef vtkIdTypeArray Superclass;
};
typedef med_int_vtkIdType_Traits<vtkIdType>::Superclass vtkMedIntArraySuperclass;
//ETX

class VTK_EXPORT vtkMedIntArray
//BTX
: public vtkMedIntArraySuperclass
//ETX
{
public :
	static vtkMedIntArray* New();
  vtkTypeRevisionMacro(vtkMedIntArray,vtkMedIntArraySuperclass);
  void PrintSelf(ostream& os, vtkIndent indent);
protected:
	vtkMedIntArray(vtkIdType numComp=1);
  ~vtkMedIntArray();
};

#endif
