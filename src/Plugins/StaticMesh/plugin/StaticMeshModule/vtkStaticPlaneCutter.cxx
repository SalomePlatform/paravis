/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkStaticPlaneCutter.cxx

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/
#include "vtkStaticPlaneCutter.h"

#include <vtkCellData.h>
#include <vtkCompositeDataIterator.h>
#include <vtkGenericCell.h>
#include <vtkIdFilter.h>
#include <vtkIdList.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkMultiBlockDataSet.h>
#include <vtkMultiPieceDataSet.h>
#include <vtkObjectFactory.h>
#include <vtkPartitionedDataSetCollection.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkUnstructuredGrid.h>
#include <vtksys/SystemTools.hxx>

vtkStandardNewMacro(vtkStaticPlaneCutter);

static const char* IdsArrayName = "__vtkSPC_Ids";

//-----------------------------------------------------------------------------
int vtkStaticPlaneCutter::RequestData(
  vtkInformation* request, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // get the inputs and outputs
  auto inputDO = vtkDataObject::GetData(inputVector[0], 0);
  auto outputDO = vtkDataObject::GetData(outputVector, 0);
  if (inputDO == nullptr)
  {
    vtkErrorMacro("Input is nullptr.");
    return 0;
  }

  vtkUnstructuredGrid* inputUG = vtkUnstructuredGrid::SafeDownCast(inputDO);
  vtkMultiBlockDataSet* inputMB = vtkMultiBlockDataSet::SafeDownCast(inputDO);
  vtkPartitionedDataSetCollection* inputPDC =
    vtkPartitionedDataSetCollection::SafeDownCast(inputDO);
  vtkPartitionedDataSet* inputPD = vtkPartitionedDataSet::SafeDownCast(inputDO);

  // Recover the first and only block/partition so this works with single block composite
  if (inputMB && inputMB->GetNumberOfBlocks() == 1)
  {
    inputUG = vtkUnstructuredGrid::SafeDownCast(inputMB->GetBlock(0));
  }
  if (inputPDC && inputPDC->GetNumberOfPartitionedDataSets() == 1)
  {
    inputPD = vtkPartitionedDataSet::SafeDownCast(inputPDC->GetPartitionedDataSet(0));
  }
  if (inputPD && inputPD->GetNumberOfPartitions() == 1)
  {
    inputUG = vtkUnstructuredGrid::SafeDownCast(inputPD->GetPartition(0));
  }

  // Recover the static unstructured grid
  if (!inputUG)
  {
    // For any other type of input, fall back to superclass implementation
    return this->Superclass::RequestData(request, inputVector, outputVector);
  }

  // Check cache validity
  if (this->InputMeshTime == inputUG->GetMeshMTime() && this->FilterMTime == this->GetMTime())
  {
    // Cache mesh is up to date, use it to generate data
    if (vtksys::SystemTools::HasEnv("VTK_DEBUG_STATIC_MESH"))
    {
      vtkWarningMacro("Using static mesh cache");
    }

    if (this->InterpolateAttributes)
    {
      // Update the cache data
      this->UpdateCacheData(inputUG);
    }

    // Copy the updated cache into the output
    this->SetOutputFromCache(outputDO);
    this->RemoveIdsArray(outputDO);
    return 1;
  }
  else
  {
    // Cache is invalid
    if (vtksys::SystemTools::HasEnv("VTK_DEBUG_STATIC_MESH"))
    {
      vtkWarningMacro("Building static mesh cache");
    }

    // Add needed Arrays
    vtkNew<vtkUnstructuredGrid> tmpInput;
    this->AddIdsArray(inputUG, tmpInput);

    // Create an input vector to pass the completed input to the superclass
    // RequestData method
    vtkNew<vtkInformationVector> tmpInputVec;
    tmpInputVec->Copy(inputVector[0], 1);
    vtkInformation* tmpInInfo = tmpInputVec->GetInformationObject(0);
    tmpInInfo->Set(vtkDataObject::DATA_OBJECT(), tmpInput);
    vtkInformationVector* tmpInputVecPt = tmpInputVec;
    int ret = this->Superclass::RequestData(request, &tmpInputVecPt, outputVector);

    // Update the cache with superclass output
    this->Cache->ShallowCopy(outputDO);
    assert(this->Cache);
    this->InputMeshTime = inputUG->GetMeshMTime();
    this->FilterMTime = this->GetMTime();

    // Compute the ids to be passed from the input to the cache
    this->ComputeIds(inputUG);
    this->RemoveIdsArray(outputDO);
    return ret;
  }
}

//-----------------------------------------------------------------------------
void vtkStaticPlaneCutter::AddIdsArray(vtkDataSet* input, vtkDataSet* output)
{
  vtkDataSet* tmpInput = input;
  vtkNew<vtkIdFilter> generateIdScalars;

  // Check for Ids array
  vtkIdTypeArray* cellIdsTmp =
    vtkIdTypeArray::SafeDownCast(input->GetCellData()->GetAbstractArray(IdsArrayName));
  if (!cellIdsTmp)
  {
    // Create Ids array
    generateIdScalars->SetInputData(tmpInput);
    generateIdScalars->SetPointIds(true);
    generateIdScalars->SetCellIds(true);
    generateIdScalars->SetPointIdsArrayName(IdsArrayName);
    generateIdScalars->SetCellIdsArrayName(IdsArrayName);
    generateIdScalars->FieldDataOn();
    generateIdScalars->Update();
    tmpInput = generateIdScalars->GetOutput();
  }
  output->ShallowCopy(tmpInput);
}

//-----------------------------------------------------------------------------
void vtkStaticPlaneCutter::RemoveIdsArray(vtkDataObject* output)
{

  if (auto pd = vtkPolyData::SafeDownCast(output))
  {
    pd->GetCellData()->RemoveArray(IdsArrayName);
  }
  else if (auto outputMP = vtkMultiPieceDataSet::SafeDownCast(output))
  {
    vtkSmartPointer<vtkCompositeDataIterator> iter;
    iter.TakeReference(outputMP->NewIterator());
    iter->SkipEmptyNodesOn();
    for (iter->GoToFirstItem(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
      vtkPolyData* slice = vtkPolyData::SafeDownCast(iter->GetCurrentDataObject());
      if (slice)
      {
        slice->GetCellData()->RemoveArray(IdsArrayName);
      }
    }
  }
  else
  {
    std::cerr << "Unhandled object type: " << output->GetClassName() << std::endl;
  }
}

//-----------------------------------------------------------------------------
void vtkStaticPlaneCutter::ComputeIds(vtkUnstructuredGrid* input)
{
  this->CellToCopyFrom.clear();
  this->CellToCopyTo.clear();
  this->WeightsVectorCompo.clear();

  const vtkIdType nbPieces = this->Cache->GetNumberOfPieces();
  this->WeightsVectorCompo.reserve(nbPieces);

  // Iterate over each piece of the multipiece output
  vtkNew<vtkGenericCell> tmpCell;
  vtkSmartPointer<vtkCompositeDataIterator> iter =
    vtkSmartPointer<vtkCompositeDataIterator>::Take(this->Cache->NewIterator());
  iter->SkipEmptyNodesOn();
  for (iter->GoToFirstItem(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
  {
    vtkPolyData* slice = vtkPolyData::SafeDownCast(iter->GetCurrentDataObject());
    vtkIdType sliceNbPoints = slice ? slice->GetNumberOfPoints() : 0;
    if (sliceNbPoints > 0)
    {
      // For each piece, recover the Ids of the cells sliced and the corresponding
      // cellId in the slice
      this->CellToCopyFrom.emplace_back();
      this->CellToCopyTo.emplace_back();
      auto& cellIdsFrom = this->CellToCopyFrom.back();
      auto& cellIdsTo = this->CellToCopyTo.back();

      vtkIdTypeArray* ids =
        vtkIdTypeArray::SafeDownCast(slice->GetCellData()->GetArray(IdsArrayName));
      assert(ids);
      cellIdsFrom->SetNumberOfIds(ids->GetNumberOfValues());
      cellIdsTo->SetNumberOfIds(ids->GetNumberOfValues());
      for (vtkIdType i = 0; i < ids->GetNumberOfValues(); i++)
      {
        cellIdsFrom->SetId(i, ids->GetValue(i));
        cellIdsTo->SetId(i, i);
      }
      if (input->GetPointData()->GetNumberOfArrays() > 0)
      {
        slice->BuildLinks();
        vtkIdType maxCellSize = input->GetMaxCellSize();
        std::vector<WeightsVectorElmt> weightsVector;
        weightsVector.resize(sliceNbPoints);
        for (vtkIdType i = 0; i < sliceNbPoints; i++)
        {
          vtkNew<vtkIdList> ptIds;
          weightsVector[i].pointsWeights.resize(maxCellSize);
          vtkIdType ncells;
          vtkIdType* cells;
          slice->GetPointCells(i, ncells, cells);
          vtkIdType cellId = cellIdsFrom->GetId(cells[0]);
          assert(cellId < input->GetNumberOfCells());
          input->GetCell(cellId, tmpCell);
          input->GetCellPoints(cellId, ptIds);
          double dist, pcoords[3], x[3], p[3];
          int subId;
          slice->GetPoint(i, p);
          tmpCell->EvaluatePosition(
            p, x, subId, pcoords, dist, weightsVector[i].pointsWeights.data());
          weightsVector[i].pointsList = ptIds;
        }
        this->WeightsVectorCompo.emplace_back(std::move(weightsVector));
      }
    }
  }
}

//-----------------------------------------------------------------------------
void vtkStaticPlaneCutter::UpdateCacheData(vtkDataSet* input)
{
  // Remove useless FieldData Array from multipiece
  // Created by automatic pass data in pipeline
  vtkFieldData* mpFieldData = this->Cache->GetFieldData();
  for (int i = mpFieldData->GetNumberOfArrays() - 1; i >= 0; i--)
  {
    mpFieldData->RemoveArray(i);
  }

  // Recover cell/point data
  vtkCellData* inCD = input->GetCellData();
  vtkPointData* inPD = input->GetPointData();

  int blockId = 0;
  vtkSmartPointer<vtkCompositeDataIterator> iter;
  iter.TakeReference(this->Cache->NewIterator());
  iter->SkipEmptyNodesOn();
  for (iter->GoToFirstItem(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
  {
    vtkPolyData* slice = vtkPolyData::SafeDownCast(iter->GetCurrentDataObject());
    if (slice && slice->GetNumberOfPoints() > 0)
    {
      vtkCellData* sliceCD = slice->GetCellData();
      for (int iArr = 0; iArr < inCD->GetNumberOfArrays(); iArr++)
      {
        // For each array of the cell data of each slice
        vtkAbstractArray* arrayToCopyIn = sliceCD->GetAbstractArray(inCD->GetArrayName(iArr));
        if (arrayToCopyIn)
        {
          // Copy the tuples from the input cell ids to the slice cell ids
          arrayToCopyIn->InsertTuples(this->CellToCopyTo[blockId], this->CellToCopyFrom[blockId],
            inCD->GetAbstractArray(iArr));
        }
      }

      if (input->GetPointData()->GetNumberOfArrays() > 0)
      {
        vtkPointData* slicePD = slice->GetPointData();
        vtkIdType sliceNbPoints = slice->GetNumberOfPoints();
        slicePD->InterpolateAllocate(inPD, sliceNbPoints);
        auto& weightsVector = this->WeightsVectorCompo[blockId];
        for (vtkIdType ptIdx = 0; ptIdx < sliceNbPoints; ptIdx++)
        {
          slicePD->InterpolatePoint(inPD, ptIdx, weightsVector[ptIdx].pointsList,
            weightsVector[ptIdx].pointsWeights.data());
        }
      }

      // Update field data
      slice->GetFieldData()->PassData(input->GetFieldData());
      blockId++;
    }
  }
}

//----------------------------------------------------------------------------
void vtkStaticPlaneCutter::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
  os << indent << "Cache: " << this->Cache << endl;
  os << indent << "Input Mesh Time: " << this->InputMeshTime << endl;
  os << indent << "Filter mTime: " << this->FilterMTime << endl;
}

//----------------------------------------------------------------------------
void vtkStaticPlaneCutter::SetOutputFromCache(vtkDataObject* output)
{
  if (auto pd = vtkPolyData::SafeDownCast(output))
  {
    assert(this->Cache->GetNumberOfPieces() == 1);
    auto p0 = vtkPolyData::SafeDownCast(this->Cache->GetPartition(0));
    assert(p0);
    pd->ShallowCopy(p0);
  }
  else if (auto mb = vtkMultiPieceDataSet::SafeDownCast(output))
  {
    mb->ShallowCopy(this->Cache);
  }
  else
  {
    vtkErrorMacro("Unhandled output type: " << output->GetClassName());
  }
}
