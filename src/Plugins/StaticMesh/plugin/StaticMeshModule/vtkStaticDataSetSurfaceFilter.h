/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkStaticDataSetSurfaceFilter.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkStaticDataSetSurfaceFilter
 * @brief   Extract the surface of a dataset, optimized for static unstructured grid
 *
 * vtkStaticDataSetSurfaceFilter is a specialization of vtkDataSetSurfaceFilter
 * that uses a cache to store the surface output and reuses it when associated data
 * changes over the time, but the geometry of a unstructured grid is static.
 * It is to be noted that, since ParaView use the same surface filter
 * for each block of a MultiBlock, this filter is not effective with multiblock
 * dataset.
 *
 * @sa
 * vtkStaticMeshObjectFactory
 */

#ifndef vtkStaticDataSetSurfaceFilter_h
#define vtkStaticDataSetSurfaceFilter_h

#include <vtkDataSetSurfaceFilter.h>
#include <vtkNew.h>

#include "StaticMeshModuleModule.h"

class vtkPolyData;

class STATICMESHMODULE_EXPORT vtkStaticDataSetSurfaceFilter : public vtkDataSetSurfaceFilter
{
public:
  static vtkStaticDataSetSurfaceFilter* New();
  // vtkTypeMacro can't be used with a factory built object
  using Superclass = vtkDataSetSurfaceFilter;
  void PrintSelf(ostream& os, vtkIndent indent) override;

  /**
   * if the input is an unstructured grid and the cache is valid, this
   * method only updates attributes (points, cells and field data)
   */
  int UnstructuredGridExecute(vtkDataSet* input, vtkPolyData* output) override;

protected:
  vtkStaticDataSetSurfaceFilter() = default;
  ~vtkStaticDataSetSurfaceFilter() override = default;

private:
  // Hide these from the user and the compiler.
  vtkStaticDataSetSurfaceFilter(const vtkStaticDataSetSurfaceFilter&) = delete;
  void operator=(const vtkStaticDataSetSurfaceFilter&) = delete;

  vtkNew<vtkPolyData> Cache;
  vtkMTimeType InputMeshTime = 0;
  vtkMTimeType FilterMTime = 0;
};

#endif
