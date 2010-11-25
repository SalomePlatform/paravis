#include "vtkSMExtractGroupProxy.h"

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
#include "vtkSMMedReaderProxy.h"

vtkStandardNewMacro(vtkSMExtractGroupProxy)
vtkCxxRevisionMacro(vtkSMExtractGroupProxy, "$Revision$")

class vtkExtractGroupProxyCommand: public vtkCommand
{
public:
  vtkExtractGroupProxyCommand()
  {
    ;
  }
  ~vtkExtractGroupProxyCommand()
  {
    ;
  }

  virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
  {
    vtkSMExtractGroupProxy* self = vtkSMExtractGroupProxy::SafeDownCast(caller);
    if(self == NULL)
      return;
    self->updateSIL();
  }
};

class vtkSMExtractGroupProxyInternal
{
public:
  vtkSMExtractGroupProxyInternal(vtkSMExtractGroupProxy* self)
  {
    this->Self = self;
    this->SIL = NULL;
    this->SILModel = vtkSmartPointer<vtkSMSILModel>::New();
    this->Observer = new vtkExtractGroupProxyCommand();
    this->Self->AddObserver(vtkCommand::UpdateInformationEvent, this->Observer);
    this->SILUpdateStamp = -1;
  }
  ~vtkSMExtractGroupProxyInternal()
  {
    this->Self->RemoveObserver(this->Observer);
  }

  vtkSmartPointer<vtkGraph> SIL;
  vtkSmartPointer<vtkSMSILModel> SILModel;
  vtkExtractGroupProxyCommand* Observer;
  vtkSMExtractGroupProxy* Self;
  vtkIdType SILUpdateStamp;
};

vtkSMExtractGroupProxy::vtkSMExtractGroupProxy()
{
  this->Internal = new vtkSMExtractGroupProxyInternal(this);
}

vtkSMExtractGroupProxy::~vtkSMExtractGroupProxy()
{
  delete this->Internal;
}

void vtkSMExtractGroupProxy::SetGroupStatus(const char* key, int status)
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

  vtkIdType meshGroupRoot = vtkSMMedReaderProxy::FindChild( vtkMedUtilities::SimplifyName(mesh.data()).data(), groupRoot, this->Internal->SILModel);
  if(entity == "*")
    {
    this->Internal->SILModel->SetCheckState(meshGroupRoot, state);
    return;
    }
  vtkIdType entityRoot = vtkSMMedReaderProxy::FindChild(entity.data(), meshGroupRoot, this->Internal->SILModel);
  if(group == "*")
    {
    this->Internal->SILModel->SetCheckState(entityRoot, state);
    return;
    }
  vtkIdType groupId = vtkSMMedReaderProxy::FindChild(vtkMedUtilities::SimplifyName(group.data()).data(), entityRoot, this->Internal->SILModel);
  this->Internal->SILModel->SetCheckState(groupId, state);
}

void vtkSMExtractGroupProxy::updateSIL()
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
    }
}

void vtkSMExtractGroupProxy::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
}
