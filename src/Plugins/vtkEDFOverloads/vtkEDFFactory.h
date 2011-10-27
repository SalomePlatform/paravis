/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkEDFFactory.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
// .NAME vtkEDFFactory -
// .SECTION Description

#ifndef __vtkEDFFactory_h
#define __vtkEDFFactory_h

#include "vtkObjectFactory.h"

class VTK_EXPORT vtkEDFFactory : public vtkObjectFactory
{
public:
// Methods from vtkObject
  vtkTypeMacro(vtkEDFFactory,vtkObjectFactory);
  static vtkEDFFactory *New();

  virtual const char* GetVTKSourceVersion();
  virtual const char* GetDescription();
protected:
  vtkEDFFactory();
private:
  vtkEDFFactory(const vtkEDFFactory&);  // Not implemented.
  void operator=(const vtkEDFFactory&);  // Not implemented.
};

extern "C" VTK_EXPORT vtkObjectFactory* vtkLoad();
#endif
