/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGenericStaticEnSightReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkGenericStaticEnSightReader.h"
#include "vtkObjectFactory.h"

#include "vtkStaticEnSight6BinaryReader.h"
#include "vtkStaticEnSight6Reader.h"
#include "vtkStaticEnSightGoldBinaryReader.h"
#include "vtkStaticEnSightGoldReader.h"

vtkStandardNewMacro(vtkGenericStaticEnSightReader);

//------------------------------------------------------------------------------
int vtkGenericStaticEnSightReader::RequestInformation(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  int ri = this->Superclass::RequestInformation(request, inputVector, outputVector);

  // 6 binary
  vtkStaticEnSight6BinaryReader* es6binary =
    vtkStaticEnSight6BinaryReader::SafeDownCast(this->Reader);
  if (es6binary)
  {
    es6binary->SetUseStaticMesh(this->UseStaticMesh);
    return ri;
  }

  // 6
  vtkStaticEnSight6Reader* es6 = vtkStaticEnSight6Reader::SafeDownCast(this->Reader);
  if (es6)
  {
    es6->SetUseStaticMesh(this->UseStaticMesh);
    return ri;
  }

  // Gold binary
  vtkStaticEnSightGoldBinaryReader* goldBinary =
    vtkStaticEnSightGoldBinaryReader::SafeDownCast(this->Reader);
  if (goldBinary)
  {
    goldBinary->SetUseStaticMesh(this->UseStaticMesh);
    return ri;
  }

  // Gold
  vtkStaticEnSightGoldReader* gold = vtkStaticEnSightGoldReader::SafeDownCast(this->Reader);
  if (gold)
  {
    gold->SetUseStaticMesh(this->UseStaticMesh);
    return ri;
  }

  vtkErrorMacro("Failed to determine static mesh version of the EnSight reader.");
  return 0;
}

//------------------------------------------------------------------------------
void vtkGenericStaticEnSightReader::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << "UseStaticMesh: " << this->UseStaticMesh << std::endl;
}
