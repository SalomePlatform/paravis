/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkStaticEnSightGoldReader.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkStaticEnSightGoldReader.h"

#include <vtkDataArray.h>
#include <vtkDataArrayCollection.h>
#include <vtkIdList.h>
#include <vtkIdListCollection.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>
#include <vtksys/SystemTools.hxx>

vtkStandardNewMacro(vtkStaticEnSightGoldReader);

//----------------------------------------------------------------------------
int vtkStaticEnSightGoldReader::RequestData(vtkInformation* vtkNotUsed(request),
  vtkInformationVector** vtkNotUsed(inputVector), vtkInformationVector* outputVector)
{
  vtkMultiBlockDataSet* output = vtkMultiBlockDataSet::GetData(outputVector);

  vtkInformation* outInfo = outputVector->GetInformationObject(0);
  int tsLength = outInfo->Length(vtkStreamingDemandDrivenPipeline::TIME_STEPS());
  double* steps = outInfo->Get(vtkStreamingDemandDrivenPipeline::TIME_STEPS());

  this->ActualTimeValue = this->TimeValue;

  // Check if a particular time was requested by the pipeline.
  // This overrides the ivar.
  if (outInfo->Has(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP()) && tsLength > 0)
  {
    // Get the requested time step. We only support requests of a single time
    // step in this reader right now
    double requestedTimeStep = outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_TIME_STEP());

    // find the first time value larger than requested time value
    // this logic could be improved
    int cnt = 0;
    while (cnt < tsLength - 1 && steps[cnt] < requestedTimeStep)
    {
      cnt++;
    }
    this->ActualTimeValue = steps[cnt];
  }

  if (!this->UseStaticMesh || this->CacheMTime < this->GetMTime())
  {
    if (vtksys::SystemTools::HasEnv("VTK_DEBUG_STATIC_MESH"))
    {
      vtkWarningMacro("Building static mesh cache");
    }

    if (!this->CaseFileRead)
    {
      vtkErrorMacro("error reading case file");
      return 0;
    }

    this->NumberOfNewOutputs = 0;
    this->NumberOfGeometryParts = 0;
    if (this->GeometryFileName)
    {
      vtkIdType timeStep = 1;
      vtkIdType timeStepInFile = 1;
      int fileNum = 1;
      std::string fileName(this->GeometryFileName);
      fileName.resize(fileName.size() + 10);

      if (this->UseTimeSets)
      {
        vtkIdType timeSet = this->TimeSetIds->IsId(this->GeometryTimeSet);
        if (timeSet >= 0)
        {
          vtkDataArray* times = this->TimeSets->GetItem(timeSet);
          this->GeometryTimeValue = times->GetComponent(0, 0);
          for (vtkIdType i = 1; i < times->GetNumberOfTuples(); i++)
          {
            double newTime = times->GetComponent(i, 0);
            if (newTime <= this->ActualTimeValue && newTime > this->GeometryTimeValue)
            {
              this->GeometryTimeValue = newTime;
              timeStep++;
              timeStepInFile++;
            }
          }
          if (this->TimeSetFileNameNumbers->GetNumberOfItems() > 0)
          {
            int collectionNum = this->TimeSetsWithFilenameNumbers->IsId(this->GeometryTimeSet);
            if (collectionNum > -1)
            {
              vtkIdList* filenameNumbers = this->TimeSetFileNameNumbers->GetItem(collectionNum);
              int filenameNum = filenameNumbers->GetId(timeStep - 1);
              if (!this->UseFileSets)
              {
                this->ReplaceWildcards(&fileName[0], filenameNum);
              }
            }
          }

          // There can only be file sets if there are also time sets.
          if (this->UseFileSets)
          {
            vtkIdType fileSet = this->FileSets->IsId(this->GeometryFileSet);
            vtkIdList* numStepsList =
              static_cast<vtkIdList*>(this->FileSetNumberOfSteps->GetItemAsObject(fileSet));

            if (timeStep > numStepsList->GetId(0))
            {
              vtkIdType numSteps = numStepsList->GetId(0);
              timeStepInFile -= numSteps;
              fileNum = 2;
              for (vtkIdType i = 1; i < numStepsList->GetNumberOfIds(); i++)
              {
                numSteps += numStepsList->GetId(i);
                if (timeStep > numSteps)
                {
                  fileNum++;
                  timeStepInFile -= numStepsList->GetId(i);
                }
              }
            }
            if (this->FileSetFileNameNumbers->GetNumberOfItems() > 0)
            {
              int collectionNum = this->FileSetsWithFilenameNumbers->IsId(this->GeometryFileSet);
              if (collectionNum > -1)
              {
                vtkIdList* filenameNumbers = this->FileSetFileNameNumbers->GetItem(collectionNum);
                int filenameNum = filenameNumbers->GetId(fileNum - 1);
                this->ReplaceWildcards(&fileName[0], filenameNum);
              }
            }
          }
        }
      }

      if (!this->ReadGeometryFile(fileName.data(), timeStepInFile, this->Cache))
      {
        vtkErrorMacro("error reading geometry file");
        return 0;
      }
    }
    if (this->MeasuredFileName)
    {
      vtkIdType timeStep = 1;
      vtkIdType timeStepInFile = 1;
      int fileNum = 1;
      std::string fileName(this->MeasuredFileName);
      fileName.resize(fileName.size() + 10);

      if (this->UseTimeSets)
      {
        vtkIdType timeSet = this->TimeSetIds->IsId(this->MeasuredTimeSet);
        if (timeSet >= 0)
        {
          vtkDataArray* times = this->TimeSets->GetItem(timeSet);
          this->MeasuredTimeValue = times->GetComponent(0, 0);
          for (vtkIdType i = 1; i < times->GetNumberOfTuples(); i++)
          {
            double newTime = times->GetComponent(i, 0);
            if (newTime <= this->ActualTimeValue && newTime > this->MeasuredTimeValue)
            {
              this->MeasuredTimeValue = newTime;
              timeStep++;
              timeStepInFile++;
            }
          }
          if (this->TimeSetFileNameNumbers->GetNumberOfItems() > 0)
          {
            int collectionNum = this->TimeSetsWithFilenameNumbers->IsId(this->MeasuredTimeSet);
            if (collectionNum > -1)
            {
              vtkIdList* filenameNumbers = this->TimeSetFileNameNumbers->GetItem(collectionNum);
              int filenameNum = filenameNumbers->GetId(timeStep - 1);
              if (!this->UseFileSets)
              {
                this->ReplaceWildcards(&fileName[0], filenameNum);
              }
            }
          }

          // There can only be file sets if there are also time sets.
          if (this->UseFileSets)
          {
            vtkIdType fileSet = this->FileSets->IsId(this->MeasuredFileSet);
            vtkIdList* numStepsList =
              static_cast<vtkIdList*>(this->FileSetNumberOfSteps->GetItemAsObject(fileSet));

            if (timeStep > numStepsList->GetId(0))
            {
              vtkIdType numSteps = numStepsList->GetId(0);
              timeStepInFile -= numSteps;
              fileNum = 2;
              for (vtkIdType i = 1; i < numStepsList->GetNumberOfIds(); i++)
              {
                numSteps += numStepsList->GetId(i);
                if (timeStep > numSteps)
                {
                  fileNum++;
                  timeStepInFile -= numStepsList->GetId(i);
                }
              }
            }
            if (this->FileSetFileNameNumbers->GetNumberOfItems() > 0)
            {
              int collectionNum = this->FileSetsWithFilenameNumbers->IsId(this->MeasuredFileSet);
              if (collectionNum > -1)
              {
                vtkIdList* filenameNumbers = this->FileSetFileNameNumbers->GetItem(fileSet);
                int filenameNum = filenameNumbers->GetId(fileNum - 1);
                this->ReplaceWildcards(&fileName[0], filenameNum);
              }
            }
          }
        }
      }
      if (!this->ReadMeasuredGeometryFile(fileName.data(), timeStepInFile, this->Cache))
      {
        vtkErrorMacro("error reading measured geometry file");
        return 0;
      }
    }
    this->CacheMTime.Modified();
  }
  output->ShallowCopy(this->Cache);

  if ((this->NumberOfVariables + this->NumberOfComplexVariables) > 0)
  {
    if (!this->ReadVariableFiles(output))
    {
      vtkErrorMacro("error reading variable files");
      return 0;
    }
  }

  return 1;
}
