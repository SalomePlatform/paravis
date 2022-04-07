/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkForceStaticMesh.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkForceStaticMesh.h"

#include <vtkCellData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkObjectFactory.h>
#include <vtkPointData.h>
#include <vtkUnstructuredGrid.h>
#include <vtksys/SystemTools.hxx>

vtkStandardNewMacro(vtkForceStaticMesh);

//------------------------------------------------------------------------------
void vtkForceStaticMesh::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "ForceCacheComputation: " << (this->ForceCacheComputation ? "on" : "off") << endl;
}

//------------------------------------------------------------------------------
int vtkForceStaticMesh::FillInputPortInformation(int, vtkInformation* info)
{
  info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkDataObject");
  return 1;
}

//------------------------------------------------------------------------------
int vtkForceStaticMesh::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkUnstructuredGrid* input = vtkUnstructuredGrid::GetData(inputVector[0]);
  vtkMultiBlockDataSet* inputMB = vtkMultiBlockDataSet::GetData(inputVector[0]);

  // Recover the first block
  if (inputMB && inputMB->GetNumberOfBlocks() >= 1)
  {
    input = vtkUnstructuredGrid::SafeDownCast(inputMB->GetBlock(0));
  }

  if (!input)
  {
    vtkErrorMacro("Input is invalid, it should be either an unstructured grid or a multiblock a unstructured grid in the first block");
    return 0;
  }

  if (inputMB && inputMB->GetNumberOfBlocks() > 1)
  {
    vtkWarningMacro("Only the first block will be passed");
  }

  vtkUnstructuredGrid* output = vtkUnstructuredGrid::GetData(outputVector);

  bool validCache = this->CacheInitialized;
  if (validCache)
  {
    if (input->GetNumberOfPoints() != this->Cache->GetNumberOfPoints())
    {
      vtkWarningMacro("Cache has been invalidated, the number of points in input changed, from " << this->Cache->GetNumberOfPoints() << " to " << input->GetNumberOfPoints());
      validCache = false;
    }
    if (input->GetNumberOfCells() != this->Cache->GetNumberOfCells())
    {
      vtkWarningMacro("Cache has been invalidated, the number of cells in input changed, from " << this->Cache->GetNumberOfCells() << " to " << input->GetNumberOfCells());
      validCache = false;
    }
  }

  if (this->ForceCacheComputation || !validCache)
  {
    // Cache is invalid
    if (vtksys::SystemTools::HasEnv("VTK_DEBUG_STATIC_MESH"))
    {
      vtkWarningMacro("Building static mesh cache");
    }

    this->Cache->DeepCopy(input);
    this->CacheInitialized = true;
  }
  else
  {
    // Cache mesh is up to date, use it to generate data
    if (vtksys::SystemTools::HasEnv("VTK_DEBUG_STATIC_MESH"))
    {
      vtkWarningMacro("Using static mesh cache");
    }

    this->Cache->GetPointData()->ShallowCopy(input->GetPointData());
    this->Cache->GetCellData()->ShallowCopy(input->GetCellData());
    this->Cache->GetFieldData()->ShallowCopy(input->GetFieldData());
  }

  output->ShallowCopy(this->Cache);
  return 1;
}
