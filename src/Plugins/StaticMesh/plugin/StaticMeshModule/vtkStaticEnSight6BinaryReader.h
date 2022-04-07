/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkStaticEnSight6BinaryReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkStaticEnSight6BinaryReader
 * @brief   class to read binary EnSight6 files
 *
 * vtkStaticEnSight6BinaryReader is a class to read binary EnSight6 files into vtk.
 * Because the different parts of the EnSight data can be of various data
 * types, this reader produces multiple outputs, one per part in the input
 * file.
 * All variable information is being stored in field data.  The descriptions
 * listed in the case file are used as the array names in the field data.
 * For complex vector variables, the description is appended with _r (for the
 * array of real values) and _i (for the array if imaginary values).  Complex
 * scalar variables are stored as a single array with 2 components, real and
 * imaginary, listed in that order.
 * @warning
 * You must manually call Update on this reader and then connect the rest
 * of the pipeline because (due to the nature of the file format) it is
 * not possible to know ahead of time how many outputs you will have or
 * what types they will be.
 * If UseStaticMesh is true, this reader generates an unstructured grid
 * with static geometry.
 */

#ifndef vtkStaticEnSight6BinaryReader_h
#define vtkStaticEnSight6BinaryReader_h

#include <vtkEnSight6BinaryReader.h>
#include <vtkNew.h>

#include "StaticMeshModuleModule.h"

class vtkMultiBlockDataSet;

class STATICMESHMODULE_EXPORT vtkStaticEnSight6BinaryReader : public vtkEnSight6BinaryReader
{
public:
  static vtkStaticEnSight6BinaryReader* New();
  vtkTypeMacro(vtkStaticEnSight6BinaryReader, vtkEnSight6BinaryReader);

  // @{
  /**
   * This boolean control whether or not the output
   * should be considered a static mesh.
   */
  vtkGetMacro(UseStaticMesh, bool);
  vtkSetMacro(UseStaticMesh, bool);
  vtkBooleanMacro(UseStaticMesh, bool);
  // @}

protected:
  vtkStaticEnSight6BinaryReader() = default;
  ~vtkStaticEnSight6BinaryReader() override = default;

  /**
   * see vtkDataSetAlgorithm
   */
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkStaticEnSight6BinaryReader(const vtkStaticEnSight6BinaryReader&) = delete;
  void operator=(const vtkStaticEnSight6BinaryReader&) = delete;

  bool UseStaticMesh = false;
  vtkNew<vtkMultiBlockDataSet> Cache;
  vtkTimeStamp CacheMTime;
};

#endif
