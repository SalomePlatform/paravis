/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkGenericStaticEnSightReader.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkGenericStaticEnSightReader
 * @brief   class to read any type of EnSight files
 *
 * The class vtkGenericStaticEnSightReader inherit from the
 * vtkGenericEnSightReader. In addition to allowing the user to read
 * various types of EnSight files, it add a boolean to control whether
 * or not the output mesh should be considered static.
 *
 * Keep this boolean to false for non-static EnSight files.
 */

#ifndef vtkGenericStaticEnSightReader_h
#define vtkGenericStaticEnSightReader_h

#include <vtkGenericEnSightReader.h>
#include <vtkSetGet.h>

#include "StaticMeshModuleModule.h"

class STATICMESHMODULE_EXPORT vtkGenericStaticEnSightReader : public vtkGenericEnSightReader
{
public:
  static vtkGenericStaticEnSightReader* New();
  vtkTypeMacro(vtkGenericStaticEnSightReader, vtkGenericEnSightReader);
  void PrintSelf(ostream& os, vtkIndent indent) override;

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
  vtkGenericStaticEnSightReader() = default;
  ~vtkGenericStaticEnSightReader() override = default;

  /**
   * see vtkDataSetAlgorithm
   */
  int RequestInformation(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;

private:
  vtkGenericStaticEnSightReader(const vtkGenericStaticEnSightReader&) = delete;
  void operator=(const vtkGenericStaticEnSightReader&) = delete;

  bool UseStaticMesh = false;
};

#endif
