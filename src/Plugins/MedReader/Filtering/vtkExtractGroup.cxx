#include "vtkExtractGroup.h"

#include "vtkObjectFactory.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformationVector.h"
#include "vtkInformation.h"
#include "vtkDataArraySelection.h"
#include "vtkMedUtilities.h"
#include "vtkTimeStamp.h"
#include "vtkOutEdgeIterator.h"
#include "vtkMedReader.h"
#include "vtkInformationDataObjectKey.h"
#include "vtkExecutive.h"
#include "vtkVariantArray.h"
#include "vtkStringArray.h"
#include "vtkUnsignedCharArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkDemandDrivenPipeline.h"

#include <vtkstd/map>
#include <vtkstd/deque>

vtkCxxRevisionMacro(vtkExtractGroup, "$Revision$");
vtkStandardNewMacro(vtkExtractGroup);

vtkCxxSetObjectMacro(vtkExtractGroup, SIL, vtkMutableDirectedGraph);

vtkExtractGroup::vtkExtractGroup()
{
  this->SIL = NULL;
  this->Entities = vtkDataArraySelection::New();
  this->Families = vtkDataArraySelection::New();
  this->PruneOutput = 0;
}

vtkExtractGroup::~vtkExtractGroup()
{
  this->Entities->Delete();
  this->Families->Delete();
}

int vtkExtractGroup::ModifyRequest(vtkInformation* request, int when)
{
  request->Set(vtkDemandDrivenPipeline::REQUEST_REGENERATE_INFORMATION(), 1);
  return this->Superclass::ModifyRequest(request, when);
}



int vtkExtractGroup::RequestInformation(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  vtkInformation* outInfo = outputVector->GetInformationObject(0);

  vtkInformation* inputInfo = inputVector[0]->GetInformationObject(0);


  if(inputInfo->Has(vtkMedReader::SELECTED_SIL()))
    {
    this->SetSIL(vtkMutableDirectedGraph::SafeDownCast(inputInfo->Get(vtkMedReader::SELECTED_SIL())));
    }
  else
    {
    vtkMutableDirectedGraph* sil = vtkMutableDirectedGraph::New();
    this->BuildDefaultSIL(sil);
    this->SetSIL(sil);
    sil->Delete();
    }
  outInfo->Set(vtkDataObject::SIL(), this->GetSIL());

  return 1;
}

void  vtkExtractGroup::BuildDefaultSIL(vtkMutableDirectedGraph* sil)
{
  sil->Initialize();

  vtkSmartPointer<vtkVariantArray> childEdge =
      vtkSmartPointer<vtkVariantArray>::New();
  childEdge->InsertNextValue(0);

  vtkSmartPointer<vtkVariantArray> crossEdge =
      vtkSmartPointer<vtkVariantArray>::New();
  crossEdge->InsertNextValue(1);

  // CrossEdge is an edge linking hierarchies.
  vtkUnsignedCharArray* crossEdgesArray = vtkUnsignedCharArray::New();
  crossEdgesArray->SetName("CrossEdges");
  sil->GetEdgeData()->AddArray(crossEdgesArray);
  crossEdgesArray->Delete();
  vtkstd::deque<vtkstd::string> names;

  // Now build the hierarchy.
  vtkIdType rootId = sil->AddVertex();
  names.push_back("SIL");

  // Add a global entry to encode global names for the families
  vtkIdType globalFamilyRoot = sil->AddChild(rootId, childEdge);
  names.push_back("Families");

  // Add the groups subtree
  vtkIdType meshesGroupRoot = sil->AddChild(rootId, childEdge);
  names.push_back("GroupTree");

  // Add the point groups subtree, to be able to globally check/uncheck all point centered supports
  vtkIdType pointSupportRoot = sil->AddChild(rootId, childEdge);
  names.push_back("PointGroups");

  // Add the point groups subtree
  vtkIdType cellSupportRoot = sil->AddChild(rootId, childEdge);
  names.push_back("CellGroups");

  // Add the attributes subtree
  vtkIdType attributesRoot = sil->AddChild(rootId, childEdge);
  names.push_back("AttributeTree");

  // Add a global entry to encode names for the cell types
  vtkIdType globalEntityRoot = sil->AddChild(rootId, childEdge);
  names.push_back("CellTypes");

  // Add the cell types subtree
  vtkIdType entityTypesRoot = sil->AddChild(rootId, childEdge);
  names.push_back("CellTypeTree");

  // This array is used to assign names to nodes.
  vtkStringArray* namesArray = vtkStringArray::New();
  namesArray->SetName("Names");
  namesArray->SetNumberOfTuples(sil->GetNumberOfVertices());
  sil->GetVertexData()->AddArray(namesArray);
  namesArray->Delete();
  vtkstd::deque<vtkstd::string>::iterator iter;
  vtkIdType cc;
  for(cc = 0, iter = names.begin(); iter != names.end(); ++iter, ++cc)
    {
    namesArray->SetValue(cc, (*iter).c_str());
    }

}

int vtkExtractGroup::RequestData(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector)

{
  vtkInformation* inputInfo = inputVector[0]->GetInformationObject(0);
  vtkMultiBlockDataSet* inmb = vtkMultiBlockDataSet::SafeDownCast(
      inputInfo->Get(vtkDataObject::DATA_OBJECT()));

  if(inmb == NULL)
    return 0;

  vtkMultiBlockDataSet* outmb = this->GetOutput();

  int nb = inmb->GetNumberOfBlocks();
  vtkInformation* meta;
  vtkInformation* outmeta;
  outmb->SetNumberOfBlocks(nb);

  for(int b = 0; b < nb; b++)
    {
    vtkMultiBlockDataSet* mesh = vtkMultiBlockDataSet::SafeDownCast(
        inmb->GetBlock(b));

    // create a block in the output that matches this mesh
    vtkMultiBlockDataSet* outMesh = vtkMultiBlockDataSet::New();
    outmb->SetBlock(b, outMesh);
    outMesh->Delete();
    meta = inmb->GetMetaData(b);
    outmeta = outmb->GetMetaData(b);
    outmeta->Copy(meta);

    std::string meshName = meta->Get(vtkCompositeDataSet::NAME());
    int npc = mesh->GetNumberOfBlocks();
    outMesh->SetNumberOfBlocks(npc);
    for(int pc = 0; pc < npc; pc++)
      {
      vtkMultiBlockDataSet* pointOrCell = vtkMultiBlockDataSet::SafeDownCast(
          mesh->GetBlock(pc));
      meta = mesh->GetMetaData(pc);

      // create a block in the output that matches this pc
      vtkMultiBlockDataSet* outPointOrCell = vtkMultiBlockDataSet::New();
      outMesh->SetBlock(pc, outPointOrCell);
      outPointOrCell->Delete();
      meta = mesh->GetMetaData(pc);
      outmeta = outMesh->GetMetaData(pc);
      outmeta->Copy(meta);

      std::string pcName = meta->Get(vtkCompositeDataSet::NAME());
      int nfam = pointOrCell->GetNumberOfBlocks();
      outPointOrCell->SetNumberOfBlocks(nfam);
      for(int fam = 0; fam < nfam; fam++)
        {
        meta = pointOrCell->GetMetaData(fam);
        outmeta = outPointOrCell->GetMetaData(fam);
        outmeta->Copy(meta);
        std::string famName = meta->Get(vtkCompositeDataSet::NAME());
        if(this->IsFamilySelected(meshName.c_str(), pcName.c_str(),
            famName.c_str()))
          {
          if(pcName == vtkMedUtilities::OnCellName)
            {
            vtkMultiBlockDataSet* family = vtkMultiBlockDataSet::SafeDownCast(
                pointOrCell->GetBlock(fam));

            vtkMultiBlockDataSet* outfamily = vtkMultiBlockDataSet::New();
            outPointOrCell->SetBlock(fam, outfamily);
            outfamily->Delete();
            int nct = family->GetNumberOfBlocks();
            outfamily->SetNumberOfBlocks(nct);
            for(int ct = 0; ct < nct; ct++)
              {
              meta = family->GetMetaData(ct);
              outmeta = outfamily->GetMetaData(ct);
              outmeta->Copy(meta);
              std::string cellTypeName = meta->Get(vtkCompositeDataSet::NAME());
              vtkDataObject* cellType = family->GetBlock(ct);
              if(cellType != NULL && this->IsCellTypeSelected(
                  cellTypeName.c_str()))
                {//accept bloc
                vtkDataObject* outCellType = cellType->NewInstance();
                outCellType->ShallowCopy(cellType);
                outfamily->SetBlock(ct, outCellType);
                outCellType->Delete();
                }
              }
            }
          else
            {
            std::string familyName = meta->Get(vtkCompositeDataSet::NAME());
            vtkDataObject* family = pointOrCell->GetBlock(fam);
            vtkDataObject* outFamily = family->NewInstance();
            outFamily->ShallowCopy(family);
            outPointOrCell->SetBlock(fam, outFamily);
            outFamily->Delete();
            }
          }
        }
      }
    }

  if(PruneOutput)
    {
    this->PruneEmptyBlocks(outmb);
    }
  return 1;
}

void vtkExtractGroup::PruneEmptyBlocks(vtkMultiBlockDataSet* mb)
{
  if(mb == NULL)
    return;
  vtkIdType nn = 0;
  while(nn < mb->GetNumberOfBlocks())
    {
    bool remove = false;
    vtkDataObject* dataObj = mb->GetBlock(nn);
    if(dataObj == NULL)
      {
      remove = true;
      }
    else
      {
      vtkMultiBlockDataSet* child = vtkMultiBlockDataSet::SafeDownCast(dataObj);
      if(child != NULL)
        {
        this->PruneEmptyBlocks(child);
        if(child->GetNumberOfBlocks() == 0)
          {
          remove = true;
          }
        }
      }
    if(remove)
      {
      mb->RemoveBlock(nn);
      }
    else
      {
      nn++;
      }
    }
}

int vtkExtractGroup::IsCellTypeSelected(const char* cellTypeKey)
{
  return this->Entities->GetArraySetting(cellTypeKey);
}

int vtkExtractGroup::IsFamilySelected(const char* meshName,
    const char* pointOrCellKey, const char* familyName)
{
  int pointOrCell =
      (strcmp(vtkMedUtilities::OnPointName, pointOrCellKey) == 0 ? vtkMedUtilities::OnPoint
          : vtkMedUtilities::OnCell);

  return this->Families->GetArraySetting(vtkMedUtilities::FamilyKey(meshName,
      pointOrCell, familyName).c_str());
}

void vtkExtractGroup::SetCellTypeStatus(const char* key, int flag)
{
  vtkIdType index = this->Entities->GetArrayIndex(key);
  int status = this->Entities->GetArraySetting(index);
  if(status != flag)
    {
    if(flag)
      {
      this->Entities->EnableArray(key);
      }
    else
      {
      this->Entities->DisableArray(key);
      }
    this->Modified();
    }
}

void vtkExtractGroup::SetFamilyStatus(const char* key, int flag)
{
  vtkIdType index = this->Families->GetArrayIndex(key);
  int status = this->Families->GetArraySetting(index);
  if(status != flag)
    {
    if(flag)
      {
      this->Families->EnableArray(key);
      }
    else
      {
      this->Families->DisableArray(key);
      }
    this->Modified();
    }
}

int vtkExtractGroup::GetSILUpdateStamp()
{
  return this->GetOutput()->GetPipelineMTime();
}

void vtkExtractGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}

