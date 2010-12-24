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
#include "vtkInEdgeIterator.h"
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

vtkCxxRevisionMacro(vtkExtractGroup, "$Revision$")
;
vtkStandardNewMacro(vtkExtractGroup)
;

vtkCxxSetObjectMacro(vtkExtractGroup, SIL, vtkMutableDirectedGraph)
;

vtkExtractGroup::vtkExtractGroup()
{
  this->SIL=NULL;
  this->Entities=vtkDataArraySelection::New();
  this->Families=vtkDataArraySelection::New();
  this->Groups=vtkDataArraySelection::New();
  this->PruneOutput=0;
}

vtkExtractGroup::~vtkExtractGroup()
{
  this->Entities->Delete();
  this->Families->Delete();
  this->Groups->Delete();
}

int vtkExtractGroup::ModifyRequest(vtkInformation* request, int when)
{
  request->Set(vtkDemandDrivenPipeline::REQUEST_REGENERATE_INFORMATION(), 1);
  return this->Superclass::ModifyRequest(request, when);
}

int vtkExtractGroup::RequestInformation(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector)
{
  vtkInformation* outInfo=outputVector->GetInformationObject(0);

  vtkInformation* inputInfo=inputVector[0]->GetInformationObject(0);

  vtkMutableDirectedGraph* old_SIL=this->GetSIL();

  if(inputInfo->Has(vtkMedReader::SELECTED_SIL()))
    {
    this->SetSIL(vtkMutableDirectedGraph::SafeDownCast(inputInfo->Get(
        vtkMedReader::SELECTED_SIL())));
    }
  else
    {
    vtkMutableDirectedGraph* sil=vtkMutableDirectedGraph::New();
    this->BuildDefaultSIL(sil);
    this->SetSIL(sil);
    sil->Delete();
    }

  if(this->GetSIL()!=old_SIL||this->GetSIL()->GetMTime()>this->SILTime)
    {
    this->ClearSelections();
    this->SILTime.Modified();
    outInfo->Set(vtkDataObject::SIL(), this->GetSIL());
    }

  return 1;
}

vtkIdType vtkExtractGroup::FindVertex(const char* name)
{
  vtkStringArray* names=vtkStringArray::SafeDownCast(
      this->GetSIL()->GetVertexData()->GetAbstractArray("Names"));

  return names->LookupValue(name);
}

void vtkExtractGroup::ClearSelections()
{
  this->Families->RemoveAllArrays();
  this->Entities->RemoveAllArrays();
  this->Groups->RemoveAllArrays();
  if(this->GetSIL()==NULL)
    return;

  vtkStringArray* names=vtkStringArray::SafeDownCast(
      this->GetSIL()->GetVertexData()->GetAbstractArray("Names"));

  vtkIdType famRoot=-1, groupRoot=-1, cellRoot=-1;
  for(vtkIdType id=0; id<names->GetNumberOfValues(); id++)
    {
    vtkstd::string name=names->GetValue(id);
    if(name=="Families")
      {
      famRoot=id;
      }
    else if(name=="Groups")
      {
      groupRoot=id;
      }
    else if(name=="CellTypes")
      {
      cellRoot=id;
      }
    }
  if(famRoot==-1||groupRoot==-1||cellRoot==-1)
    {
    return;
    }

  vtkOutEdgeIterator* it=vtkOutEdgeIterator::New();
  this->GetSIL()->GetOutEdges(famRoot, it);
  while(it->HasNext())
    {
    vtkIdType fam=it->Next().Target;
    vtkStdString name=names->GetValue(fam);
    this->Families->AddArray(name);
    if(name.find("/OnPoint/")!=std::string::npos)
      {
      this->Families->DisableArray(name.c_str());
      }
    }

  this->GetSIL()->GetOutEdges(groupRoot, it);
  while(it->HasNext())
    {
    vtkIdType group=it->Next().Target;
    vtkStdString name=names->GetValue(group);
    this->Groups->AddArray(name);
    if(name.find("/OnPoint/")!=std::string::npos)
      {
      this->Groups->DisableArray(name.c_str());
      }
    }
  this->GroupSelectionTime.Modified();

  this->GetSIL()->GetOutEdges(cellRoot, it);
  while(it->HasNext())
    {
    vtkIdType cell=it->Next().Target;
    this->Entities->AddArray(names->GetValue(cell));
    }

  it->Delete();
}

void vtkExtractGroup::BuildDefaultSIL(vtkMutableDirectedGraph* sil)
{
  sil->Initialize();

  vtkSmartPointer<vtkVariantArray> childEdge=
      vtkSmartPointer<vtkVariantArray>::New();
  childEdge->InsertNextValue(0);

  vtkSmartPointer<vtkVariantArray> crossEdge=
      vtkSmartPointer<vtkVariantArray>::New();
  crossEdge->InsertNextValue(1);

  // CrossEdge is an edge linking hierarchies.
  vtkUnsignedCharArray* crossEdgesArray=vtkUnsignedCharArray::New();
  crossEdgesArray->SetName("CrossEdges");
  sil->GetEdgeData()->AddArray(crossEdgesArray);
  crossEdgesArray->Delete();
  vtkstd::deque<vtkstd::string> names;

  // Now build the hierarchy.
  vtkIdType rootId=sil->AddVertex();
  names.push_back("SIL");

  // Add a global entry to encode global names for the families
  vtkIdType globalFamilyRoot=sil->AddChild(rootId, childEdge);
  names.push_back("Families");

  // Add a global entry to encode global names for the families
  vtkIdType globalGroupRoot=sil->AddChild(rootId, childEdge);
  names.push_back("Groups");

  // Add the groups subtree
  vtkIdType groupsRoot=sil->AddChild(rootId, childEdge);
  names.push_back("GroupTree");

  // Add the attributes subtree
  vtkIdType attributesRoot=sil->AddChild(rootId, childEdge);
  names.push_back("Attributes");

  // Add a global entry to encode names for the cell types
  vtkIdType globalEntityRoot=sil->AddChild(rootId, childEdge);
  names.push_back("CellTypes");

  // Add the cell types subtree
  vtkIdType entityTypesRoot=sil->AddChild(rootId, childEdge);
  names.push_back("CellTypeTree");

  // This array is used to assign names to nodes.
  vtkStringArray* namesArray=vtkStringArray::New();
  namesArray->SetName("Names");
  namesArray->SetNumberOfTuples(sil->GetNumberOfVertices());
  sil->GetVertexData()->AddArray(namesArray);
  namesArray->Delete();
  vtkstd::deque<vtkstd::string>::iterator iter;
  vtkIdType cc;
  for(cc=0, iter=names.begin(); iter!=names.end(); ++iter, ++cc)
    {
    namesArray->SetValue(cc, (*iter).c_str());
    }

}

int vtkExtractGroup::RequestData(vtkInformation *request,
    vtkInformationVector **inputVector, vtkInformationVector *outputVector)

{
  vtkInformation* inputInfo=inputVector[0]->GetInformationObject(0);
  vtkMultiBlockDataSet* inmb=vtkMultiBlockDataSet::SafeDownCast(inputInfo->Get(
      vtkDataObject::DATA_OBJECT()));

  if(inmb==NULL)
    return 0;

  vtkMultiBlockDataSet* outmb=this->GetOutput();

  int nb=inmb->GetNumberOfBlocks();
  vtkInformation* meta;
  vtkInformation* outmeta;
  outmb->SetNumberOfBlocks(nb);

  for(int b=0; b<nb; b++)
    {
    vtkMultiBlockDataSet* mesh=vtkMultiBlockDataSet::SafeDownCast(
        inmb->GetBlock(b));

    // create a block in the output that matches this mesh
    vtkMultiBlockDataSet* outMesh=vtkMultiBlockDataSet::New();
    outmb->SetBlock(b, outMesh);
    outMesh->Delete();
    meta=inmb->GetMetaData(b);
    outmeta=outmb->GetMetaData(b);
    outmeta->Copy(meta);

    std::string meshName=meta->Get(vtkCompositeDataSet::NAME());
    int npc=mesh->GetNumberOfBlocks();
    outMesh->SetNumberOfBlocks(npc);
    for(int pc=0; pc<npc; pc++)
      {
      vtkMultiBlockDataSet* pointOrCell=vtkMultiBlockDataSet::SafeDownCast(
          mesh->GetBlock(pc));
      meta=mesh->GetMetaData(pc);

      // create a block in the output that matches this pc
      vtkMultiBlockDataSet* outPointOrCell=vtkMultiBlockDataSet::New();
      outMesh->SetBlock(pc, outPointOrCell);
      outPointOrCell->Delete();
      meta=mesh->GetMetaData(pc);
      outmeta=outMesh->GetMetaData(pc);
      outmeta->Copy(meta);

      std::string pcName=meta->Get(vtkCompositeDataSet::NAME());
      int nfam=pointOrCell->GetNumberOfBlocks();
      outPointOrCell->SetNumberOfBlocks(nfam);
      for(int fam=0; fam<nfam; fam++)
        {
        meta=pointOrCell->GetMetaData(fam);
        outmeta=outPointOrCell->GetMetaData(fam);
        outmeta->Copy(meta);
        std::string famName=meta->Get(vtkCompositeDataSet::NAME());
        if(this->IsFamilySelected(meshName.c_str(), pcName.c_str(),
            famName.c_str()))
          {
          if(pcName==vtkMedUtilities::OnCellName)
            {
            vtkMultiBlockDataSet* family=vtkMultiBlockDataSet::SafeDownCast(
                pointOrCell->GetBlock(fam));

            vtkMultiBlockDataSet* outfamily=vtkMultiBlockDataSet::New();
            outPointOrCell->SetBlock(fam, outfamily);
            outfamily->Delete();
            int nct=family->GetNumberOfBlocks();
            outfamily->SetNumberOfBlocks(nct);
            for(int ct=0; ct<nct; ct++)
              {
              meta=family->GetMetaData(ct);
              outmeta=outfamily->GetMetaData(ct);
              outmeta->Copy(meta);
              std::string cellTypeName=meta->Get(vtkCompositeDataSet::NAME());
              vtkDataObject* cellType=family->GetBlock(ct);
              if(cellType!=NULL&&this->IsCellTypeSelected(cellTypeName.c_str()))
                {//accept bloc
                vtkDataObject* outCellType=cellType->NewInstance();
                outCellType->ShallowCopy(cellType);
                outfamily->SetBlock(ct, outCellType);
                outCellType->Delete();
                }
              }
            }
          else
            {
            std::string familyName=meta->Get(vtkCompositeDataSet::NAME());
            vtkDataObject* family=pointOrCell->GetBlock(fam);
            vtkDataObject* outFamily=family->NewInstance();
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

void vtkExtractGroup::SetGroupStatus(const char* key, int flag)
{
  vtkIdType index=this->Groups->GetArrayIndex(key);
  if(index==-1)
    {
    return;
    }
  int status=this->Groups->GetArraySetting(index);
  if(status!=flag)
    {
    if(flag)
      {
      this->Groups->EnableArray(key);
      }
    else
      {
      this->Groups->DisableArray(key);
      }
    this->Modified();
    }
  this->GroupSelectionTime.Modified();
}

void vtkExtractGroup::PruneEmptyBlocks(vtkMultiBlockDataSet* mb)
{
  if(mb==NULL)
    return;
  vtkIdType nn=0;
  while(nn<mb->GetNumberOfBlocks())
    {
    bool remove=false;
    vtkDataObject* dataObj=mb->GetBlock(nn);
    if(dataObj==NULL)
      {
      remove=true;
      }
    else
      {
      vtkMultiBlockDataSet* child=vtkMultiBlockDataSet::SafeDownCast(dataObj);
      if(child!=NULL)
        {
        this->PruneEmptyBlocks(child);
        if(child->GetNumberOfBlocks()==0)
          {
          remove=true;
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
  if(this->FamilySelectionTime <= this->GroupSelectionTime)
    {
    this->SelectFamiliesFromGroups();
    }

  int
      pointOrCell=
          (strcmp(vtkMedUtilities::OnPointName, pointOrCellKey)==0? vtkMedUtilities::OnPoint
              : vtkMedUtilities::OnCell);

  std::string name=
      vtkMedUtilities::FamilyKey(meshName, pointOrCell, familyName);

  return this->Families->GetArraySetting(name.c_str());
}

void vtkExtractGroup::SelectFamiliesFromGroups()
{
  this->Families->DisableAllArrays();
  vtkStringArray* names=vtkStringArray::SafeDownCast(
      this->GetSIL()->GetVertexData()->GetAbstractArray("Names"));

  for(int index = 0; index < this->Groups->GetNumberOfArrays(); index++)
    {
    if(this->Groups->GetArraySetting(index) == 0)
      continue;

    const char* name = this->Groups->GetArrayName(index);
    vtkIdType silindex = this->FindVertex(name);

    vtkOutEdgeIterator* it = vtkOutEdgeIterator::New();

    this->GetSIL()->GetOutEdges(silindex, it);
    while(it->HasNext())
      {
      vtkIdType famId = it->Next().Target;
      vtkStdString famName = names->GetValue(famId);
      this->Families->EnableArray(famName.c_str());
      }
    it->Delete();
    }

  this->FamilySelectionTime.Modified();
}

void vtkExtractGroup::SetCellTypeStatus(const char* key, int flag)
{
  vtkIdType index=this->Entities->GetArrayIndex(key);
  if(index==-1)
    return;
  int status=this->Entities->GetArraySetting(index);
  if(status!=flag)
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
  vtkIdType index=this->Families->GetArrayIndex(key);
  if(index==-1)
    {
    return;
    }
  int status=this->Families->GetArraySetting(index);
  if(status!=flag)
    {
    if(flag)
      {
      this->Families->EnableArray(key);
      }
    else
      {
      this->Families->DisableArray(key);
      }
    }
}

int vtkExtractGroup::GetSILUpdateStamp()
{
  return this->SILTime;
}

void vtkExtractGroup::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent);
}
