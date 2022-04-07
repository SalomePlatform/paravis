/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkTemporalUGWavelet.h

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
/**
 * @class   vtkTemporalUGWavelet
 * @brief   Create a Temporal Unstructured Grid Wavelet with a static mesh
 *
 * vtkTemporalUGWavelet specialize vtkRTAnalyticSource to create
 * a wavelet converted to vtkUnstructuredGrid, with timesteps.
 * The "tPoint" and "tCell" arrays are the only data actually changing
 * over time, the geometry is generated once and then not modified.
 */

#ifndef vtkTemporalUGWavelet_h
#define vtkTemporalUGWavelet_h

#include <vtkNew.h>
#include <vtkRTAnalyticSource.h>

#include "StaticMeshModuleModule.h"

class vtkUnstructuredGrid;

class STATICMESHMODULE_EXPORT vtkTemporalUGWavelet : public vtkRTAnalyticSource
{
public:
  static vtkTemporalUGWavelet* New();
  vtkTypeMacro(vtkTemporalUGWavelet, vtkRTAnalyticSource);
  void PrintSelf(ostream& os, vtkIndent indent) override;

  //@{
  /**
   * Set/Get the number of time steps. Initial value is 10.
   */
  vtkSetMacro(NumberOfTimeSteps, int);
  vtkGetMacro(NumberOfTimeSteps, int);
  //@}

protected:
  vtkTemporalUGWavelet() = default;
  ~vtkTemporalUGWavelet() override = default;

  //@{
  /**
   * see vtkDataSetAlgorithm
   */
  int FillOutputPortInformation(int vtkNotUsed(port), vtkInformation* info) override;
  int RequestInformation(vtkInformation* request, vtkInformationVector** inputVector,
    vtkInformationVector* outputVector) override;
  int RequestData(vtkInformation*, vtkInformationVector**, vtkInformationVector*) override;
  //@}

private:
  vtkTemporalUGWavelet(const vtkTemporalUGWavelet&) = delete;
  void operator=(const vtkTemporalUGWavelet&) = delete;

  int NumberOfTimeSteps = 10;
  vtkNew<vtkUnstructuredGrid> Cache;
  vtkTimeStamp CacheMTime;
};

#endif
