/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkEDFFactory.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkEDFFactory.h"

#include "vtkVersion.h"

#include "vtkEDFCutter.h"

vtkStandardNewMacro(vtkEDFFactory);

VTK_CREATE_CREATE_FUNCTION(vtkEDFCutter);

vtkEDFFactory::vtkEDFFactory()
{
  this->RegisterOverride("vtkCutter",
                         "vtkEDFCutter",
                         "MergeTriangles",
                         1,
                         vtkObjectFactoryCreatevtkEDFCutter);
}

const char* vtkEDFFactory::GetVTKSourceVersion()
{
  return VTK_SOURCE_VERSION;
}

const char* vtkEDFFactory::GetDescription()
{
  return "VTK EDF Factory";
}

VTK_FACTORY_INTERFACE_IMPLEMENT(vtkEDFFactory);
