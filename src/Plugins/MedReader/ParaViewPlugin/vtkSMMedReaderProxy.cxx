#include "vtkSMMedReaderProxy.h"

#include "vtkObjectFactory.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkProcessModule.h"
#include "vtkPVSILInformation.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkCommand.h"
#include "vtkStringArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkSMSILModel.h"
#include "vtkMedUtilities.h"
#include "vtkSMPropertyHelper.h"

vtkStandardNewMacro(vtkSMMedReaderProxy)
vtkCxxRevisionMacro(vtkSMMedReaderProxy, "$Revision$")

class vtkMedReaderProxyCommand: public vtkCommand
{
public:
  vtkMedReaderProxyCommand()
  {
    ;
  }
  ~vtkMedReaderProxyCommand()
  {
    ;
  }

  virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
  {
    vtkSMMedReaderProxy* self = vtkSMMedReaderProxy::SafeDownCast(caller);
    if(self == NULL)
      return;
    self->updateSIL();
  }
};

class vtkSMMedReaderProxyInternal
{
public:
  vtkSMMedReaderProxyInternal(vtkSMMedReaderProxy* self)
  {
    this->Self = self;
    this->SIL = NULL;
    this->SILModel = vtkSmartPointer<vtkSMSILModel>::New();
    this->Observer = new vtkMedReaderProxyCommand();
    this->Self->AddObserver(vtkCommand::UpdateInformationEvent, this->Observer);
    this->SILUpdateStamp = -1;
  }
  ~vtkSMMedReaderProxyInternal()
  {
    this->Self->RemoveObserver(this->Observer);
  }

  vtkSmartPointer<vtkGraph> SIL;
  vtkSmartPointer<vtkSMSILModel> SILModel;
  vtkMedReaderProxyCommand* Observer;
  vtkSMMedReaderProxy* Self;
  vtkIdType SILUpdateStamp;
};

vtkSMMedReaderProxy::vtkSMMedReaderProxy()
{
  this->Internal = new vtkSMMedReaderProxyInternal(this);
}

vtkSMMedReaderProxy::~vtkSMMedReaderProxy()
{
  delete this->Internal;
}

vtkIdType vtkSMMedReaderProxy::FindChild(const char* name, vtkIdType parent, vtkSMSILModel* model)
{
  vtkstd::set<vtkIdType> leaves;
  model->GetLeaves(leaves, parent, false);
  vtkstd::set<vtkIdType>::iterator it = leaves.begin();
  while(it != leaves.end())
    {
    vtkIdType child = *it;
    if(strcmp(model->GetName(child), name) == 0)
      return child;
    it++;
    }
  return -1;
}

void vtkSMMedReaderProxy::SetGroupStatus(const char* key, int status)
{
  if(this->Internal->SIL == NULL || this->Internal->SILModel)
    return;

  vtkstd::string mesh, entity, group;
  vtkMedUtilities::SplitGroupKey(key, mesh, entity, group);

  int state = (status != 0? vtkSMSILModel::CHECKED : vtkSMSILModel::UNCHECKED);

  vtkIdType groupRoot = this->Internal->SILModel->FindVertex("GroupTree");
  if(mesh == "*")
    {
    this->Internal->SILModel->SetCheckState(groupRoot, state);
    return;
  }

  vtkIdType meshGroupRoot = FindChild(vtkMedUtilities::SimplifyName(mesh.data()).data(), groupRoot, this->Internal->SILModel);
  if(entity == "*")
    {
    this->Internal->SILModel->SetCheckState(meshGroupRoot, state);
    return;
    }
  vtkIdType entityRoot = FindChild(entity.data(), meshGroupRoot, this->Internal->SILModel);
  if(group == "*")
    {
    this->Internal->SILModel->SetCheckState(entityRoot, state);
    return;
    }
  vtkIdType groupId = FindChild(vtkMedUtilities::SimplifyName(group.data()).data(), entityRoot, this->Internal->SILModel);
  this->Internal->SILModel->SetCheckState(groupId, state);
}

void vtkSMMedReaderProxy::updateSIL()
{
  this->UpdatePropertyInformation();

  int stamp = vtkSMPropertyHelper(this, "SILUpdateStamp").GetAsInt();
  if(stamp != this->Internal->SILUpdateStamp)
    {
    this->Internal->SILUpdateStamp = stamp;

    vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
    vtkPVSILInformation* info = vtkPVSILInformation::New();
    pm->GatherInformation(this->GetConnectionID(),
        vtkProcessModule::DATA_SERVER, info, this->GetID());

    vtkMutableDirectedGraph* sil = vtkMutableDirectedGraph::SafeDownCast(
        info->GetSIL());

    this->Internal->SIL = sil;
    this->Internal->SILModel->Initialize(this,
        vtkSMStringVectorProperty::SafeDownCast(this->GetProperty(
            "Families")));

    InitializeSelections();
    }
}

void vtkSMMedReaderProxy::InitializeSelections()
{
  if(this->Internal->SIL.GetPointer() == NULL)
    return;

  this->Internal->SILModel->SetCheckState("SIL", vtkSMSILModel::CHECKED);
  this->Internal->SILModel->SetCheckState("PointGroups", vtkSMSILModel::UNCHECKED);
}

void vtkSMMedReaderProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
}
