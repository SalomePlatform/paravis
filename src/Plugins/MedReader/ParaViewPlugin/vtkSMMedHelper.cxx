#include "vtkSMMedHelper.h"

#include "vtkObjectFactory.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkMutableDirectedGraph.h"
#include "vtkCommand.h"
#include "vtkSMSILModel.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMProxy.h"
#include "vtkSMPropertyAdaptor.h"
#include "vtkSmartPointer.h"
#include "vtkPVSILInformation.h"
#include "vtkProcessModule.h"

vtkStandardNewMacro(vtkSMMedHelper)
vtkCxxRevisionMacro(vtkSMMedHelper, "$Revision$")

class vtkMedHelperCommand: public vtkCommand
{
public:
  vtkMedHelperCommand(vtkSMMedHelper* helper) :
    Helper(helper)
  {
    ;
  }
  ~vtkMedHelperCommand()
  {
    ;
  }

  virtual void Execute(vtkObject *caller, unsigned long eventId, void *callData)
  {
    if(this->Helper==NULL)
      return;
    this->Helper->updateSIL();
  }

  vtkSMMedHelper* Helper;
};

class vtkSMMedHelperInternal
{
public:
  vtkSMMedHelperInternal(vtkSMMedHelper* self)
  {
    this->Self=self;
    this->Proxy=NULL;
    this->SIL=NULL;
    this->SILModel=vtkSmartPointer<vtkSMSILModel>::New();
    this->SILUpdateStamp=-1;
    this->Observer=new vtkMedHelperCommand(self);
  }
  ~vtkSMMedHelperInternal()
  {
    this->Observer->UnRegister();
  }

  vtkSmartPointer<vtkGraph> SIL;
  vtkSmartPointer<vtkSMSILModel> SILModel;
  vtkIdType SILUpdateStamp;
  vtkMedHelperCommand* Observer;
  vtkSMMedHelper* Self;
  vtkSMProxy* Proxy;
};

vtkSMMedHelper::vtkSMMedHelper()
{
  this->Internal=new vtkSMMedHelperInternal(this);
}

vtkSMMedHelper::~vtkSMMedHelper()
{
  this->SetProxy(NULL);
  delete this->Internal;
}

void vtkSMMedHelper::SetProxy(vtkSMProxy* proxy)
{
  if(this->Internal->Proxy!=NULL)
    {
    this->Internal->Proxy->RemoveObserver(this->Internal->Observer);
    }

  this->Internal->Proxy=proxy;
  if(proxy!=NULL)
    {
    this->Internal->Proxy->AddObserver(vtkCommand::UpdateInformationEvent,
        this->Internal->Observer);
    }
}

void vtkSMMedHelper::updateSIL()
{
  if(this->Internal->Proxy==NULL)
    return;

  this->Internal->Proxy->UpdatePropertyInformation();

  int stamp=
      vtkSMPropertyHelper(this->Internal->Proxy, "SILUpdateStamp").GetAsInt();
  if(stamp!=this->Internal->SILUpdateStamp)
    {

    this->Internal->SILUpdateStamp=stamp;

    vtkProcessModule* pm=vtkProcessModule::GetProcessModule();
    vtkPVSILInformation* info=vtkPVSILInformation::New();
    pm->GatherInformation(this->Internal->Proxy->GetConnectionID(),
        vtkProcessModule::DATA_SERVER, info, this->Internal->Proxy->GetID());

    vtkMutableDirectedGraph* sil=vtkMutableDirectedGraph::SafeDownCast(
        info->GetSIL());

    this->Internal->SIL=sil;
    this->Internal->SILModel->Initialize(this->Internal->Proxy,
        vtkSMStringVectorProperty::SafeDownCast(
            this->Internal->Proxy->GetProperty("Groups")));

    this->InitializeSelections();

    }
}

void vtkSMMedHelper::InitializeSelections()
{
  if(this->Internal->SIL.GetPointer()==NULL)
    return;

  vtkIdType root=this->Internal->SILModel->FindVertex("GroupTree");

  for(int mesh_id=0; mesh_id
      <this->Internal->SILModel->GetNumberOfChildren(root); mesh_id++)
    {
    vtkIdType mesh=this->Internal->SILModel->GetChildVertex(root, mesh_id);
    if(mesh==-1)
      continue;

    for(int child_id=0; child_id<this->Internal->SILModel->GetNumberOfChildren(
        mesh_id); child_id++)
      {
      vtkIdType child=this->Internal->SILModel->GetChildVertex(mesh, child_id);
      if(child==-1)
        continue;

      const char* name=this->Internal->SILModel->GetName(child);

      if(strcmp(this->Internal->SILModel->GetName(child), "OnPoint")==0)
        {
        this->Internal->SILModel->SetCheckState(child, vtkSMSILModel::UNCHECKED);
        }
      else if(strcmp(this->Internal->SILModel->GetName(child), "OnCell")==0)
        {
        this->Internal->SILModel->SetCheckState(child, vtkSMSILModel::CHECKED);
        }
      }
    }

  vtkSMPropertyAdaptor* adaptor = vtkSMPropertyAdaptor::New();
  vtkSMStringVectorProperty* gprop = vtkSMStringVectorProperty::SafeDownCast(
      this->Internal->Proxy->GetProperty("Groups"));
  adaptor->SetProperty(gprop);

  vtkstd::set<vtkIdType> groups;
  this->Internal->SILModel->GetLeaves(groups, root, false);
  vtkstd::set<vtkIdType>::iterator it=groups.begin();

  for(int idx = 0; idx < adaptor->GetNumberOfSelectionElements(); idx++)
    {
    const char* name = adaptor->GetSelectionName(idx);
    vtkIdType groupId = this->Internal->SILModel->FindVertex(name);
    if(groupId == -1 || strncmp(name, "GROUP", strlen("GROUP")) != 0)
      continue;

    int status = this->Internal->SILModel->GetCheckStatus(groupId);
    adaptor->SetSelectionValue(idx, (status? "1" : "0"));
    }
  adaptor->Delete();
}

void vtkSMMedHelper::PrintSelf(ostream& os, vtkIndent indent)
{
  this->Superclass::PrintSelf(os, indent.GetNextIndent());
}
