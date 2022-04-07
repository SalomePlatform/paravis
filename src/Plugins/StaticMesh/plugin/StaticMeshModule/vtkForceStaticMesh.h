/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkForceStaticMesh.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class vtkForceStaticMesh
 * @brief Takes in input as a cache the first time it is executed then use it as a static mesh
 *
 * The Force Static Mesh filter create a cache the first time it is used using its input. It will then only
 * update PointData, CellData and FieldData from the input if their dimensions are valid.
 * This filter operates only on vtkUnstructuredGrid or on vtkMultiBlockDataSet containing a single vtkUnstructuredGrid
 */

#ifndef vtkForceStaticMesh_h
#define vtkForceStaticMesh_h

#include <vtkUnstructuredGridAlgorithm.h>
#include <vtkNew.h>

#include "StaticMeshModuleModule.h"

class vtkUnstructuredGrid;

class STATICMESHMODULE_EXPORT vtkForceStaticMesh : public vtkUnstructuredGridAlgorithm
{
public:
  static vtkForceStaticMesh* New();
  vtkTypeMacro(vtkForceStaticMesh, vtkUnstructuredGridAlgorithm);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * When set to true, this will force this filter to recompute its cache.
   * Default is false.
   */
  vtkSetMacro(ForceCacheComputation, vtkTypeBool);
  vtkGetMacro(ForceCacheComputation, vtkTypeBool);
  vtkBooleanMacro(ForceCacheComputation, vtkTypeBool);
  //@}

protected:
  vtkForceStaticMesh() = default;
  ~vtkForceStaticMesh() override = default;

  int FillInputPortInformation(int port, vtkInformation* info) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkForceStaticMesh(const vtkForceStaticMesh&) = delete;
  void operator=(const vtkForceStaticMesh&) = delete;
  
  bool ForceCacheComputation = false;
  bool CacheInitialized = false;
  vtkNew<vtkUnstructuredGrid> Cache;
};

#endif
