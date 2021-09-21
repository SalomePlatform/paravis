#include "vtkExtractCellType.h"

#include "vtkCell.h"
#include "vtkCellTypes.h"
#include "vtkCompositeDataSet.h"
#include "vtkCompositeDataIterator.h"
#include "vtkDataArraySelection.h"
#include "vtkDataSet.h"
#include "vtkExtractSelection.h"
#include "vtkIdTypeArray.h"
#include "vtkInformation.h"
#include "vtkObjectFactory.h"
#include "vtkSelection.h"
#include "vtkSelectionNode.h"

vtkStandardNewMacro(vtkExtractCellType);

//----------------------------------------------------------------------------
vtkExtractCellType::vtkExtractCellType(){}

//----------------------------------------------------------------------------
vtkExtractCellType::~vtkExtractCellType(){}

//----------------------------------------------------------------------------
void vtkExtractCellType::AppendToGeoTypes(vtkDataSet* dataset)
{
  for(int i = 0; i < dataset->GetNumberOfCells(); i++)
  {
    vtkCell* cell = dataset->GetCell(i);

    // TODO use MEDCoupling CellType API
    const char* cellType = vtkCellTypes::GetClassNameFromTypeId(cell->GetCellType());
    if (!this->FieldSelection->ArrayExists(cellType))
    {
      this->FieldSelection->AddArray(cellType, false);
    }
  }
}

//----------------------------------------------------------------------------
void vtkExtractCellType::SelectIds(vtkDataSet* dataset, vtkIdTypeArray* selArr)
{
  for(int i = 0; i < dataset->GetNumberOfCells(); i++)
  {
    vtkCell* cell = dataset->GetCell(i);
    
    // TODO use MEDCoupling CellType API
    const char* cellType = vtkCellTypes::GetClassNameFromTypeId(cell->GetCellType());

    // Needed as append to geotypes may not have been done in request information
    // when loading state
    if (!this->FieldSelection->ArrayExists(cellType))
    {
      this->FieldSelection->AddArray(cellType, false);
    }
    else if(this->FieldSelection->ArrayIsEnabled(cellType))
    {
      selArr->InsertNextValue(i);
    }
  }
}

//----------------------------------------------------------------------------
int vtkExtractCellType::RequestInformation(vtkInformation* req, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  vtkDataSet* inputDS = vtkDataSet::GetData(inputVector[0]);
  vtkCompositeDataSet* inputComposite = vtkCompositeDataSet::GetData(inputVector[0]);

  if (inputDS)
  {
    this->AppendToGeoTypes(inputDS);
  }
  else if(inputComposite)
  {
    vtkSmartPointer<vtkCompositeDataIterator> iter;
    iter.TakeReference(inputComposite->NewIterator());
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
      this->AppendToGeoTypes(vtkDataSet::SafeDownCast(inputComposite->GetDataSet(iter)));
    }
  }

  return 1;
}

//----------------------------------------------------------------------------
int vtkExtractCellType::RequestData(vtkInformation* req, vtkInformationVector** inputVector, vtkInformationVector* outputVector)
{
  // Get the input and output data objects.
  vtkDataSet* input = vtkDataSet::GetData(inputVector[0]);
  vtkDataSet* output = vtkDataSet::GetData(outputVector);

  // Create a selection, sel1, of cells with indices 7643-7499-7355-7211
  vtkNew<vtkIdTypeArray> selArr1;

  vtkDataSet* inputDS = vtkDataSet::GetData(inputVector[0]);
  vtkCompositeDataSet* inputComposite = vtkCompositeDataSet::GetData(inputVector[0]);

  if (inputDS)
  {
    this->SelectIds(inputDS, selArr1);
  }
  else if(inputComposite)
  {
    vtkSmartPointer<vtkCompositeDataIterator> iter;
    iter.TakeReference(inputComposite->NewIterator());
    for (iter->InitTraversal(); !iter->IsDoneWithTraversal(); iter->GoToNextItem())
    {
      this->SelectIds(vtkDataSet::SafeDownCast(inputComposite->GetDataSet(iter)), selArr1);
    }
  }

  vtkNew<vtkSelectionNode> selNode1;
  selNode1->SetContentType(vtkSelectionNode::INDICES);
  selNode1->SetFieldType(vtkSelectionNode::CELL);
  selNode1->GetProperties()->Set(vtkSelectionNode::COMPOSITE_INDEX(), 1);
  selNode1->GetProperties()->Set(vtkSelectionNode::INVERSE(), this->InsideOut);
  selNode1->SetSelectionList(selArr1);
  vtkNew<vtkSelection> sel1;
  sel1->AddNode(selNode1);

  this->SetInputData(1, sel1);
  this->Superclass::RequestData(req, inputVector, outputVector);
  return 1;
}

//----------------------------------------------------------------------------
void vtkExtractCellType::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
//------------------------------------------------------------------------------
int vtkExtractCellType::GetNumberOfGeoTypesArrays()
{
  return this->FieldSelection->GetNumberOfArrays();
}

//------------------------------------------------------------------------------
const char *vtkExtractCellType::GetGeoTypesArrayName(int index)
{
  return this->FieldSelection->GetArrayName(index) + 3;  // String parsing -- remove "vtk"
}

//------------------------------------------------------------------------------
int vtkExtractCellType::GetGeoTypesArrayStatus(const char *name)
{
  return this->FieldSelection->ArrayIsEnabled(name);
}

//------------------------------------------------------------------------------
void vtkExtractCellType::SetGeoTypesStatus(const char *name, int status)
{
  std::string complete_name = "vtk";
  complete_name.append(name);
  if (this->GetGeoTypesArrayStatus(complete_name.c_str()) != status)
  {
    if (status)
    {
      this->FieldSelection->EnableArray(complete_name.c_str());
    }
    else
    {
      this->FieldSelection->DisableArray(complete_name.c_str());
    }
    this->Modified();
  }
}
