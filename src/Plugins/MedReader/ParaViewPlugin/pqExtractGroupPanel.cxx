#include "pqExtractGroupPanel.h"
#include "ui_ExtractGroupPanel.h"

#include "vtkProcessModule.h"
#include "vtkMultiBlockDataSet.h"
#include "vtkInformation.h"
#include "vtkIntArray.h"
#include "vtkSMDoubleVectorProperty.h"
#include "vtkSMIntVectorProperty.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkSMProxy.h"
#include "vtkEventQtSlotConnect.h"
#include "vtkPVSILInformation.h"
#include "vtkGraph.h"
#include "vtkSMPropertyHelper.h"
#include "vtkSMDoubleArrayInformationHelper.h"
#include "vtkStringArray.h"
#include "vtkDataSetAttributes.h"
#include "vtkProcessModuleConnectionManager.h"

#include "vtkMedUtilities.h"
#include "vtkSMExtractGroupProxy.h"

#include "pqTreeWidget.h"
#include "pqTreeWidgetItemObject.h"
#include "pqSMAdaptor.h"
#include "pqProxy.h"
#include "pqPropertyManager.h"
#include "pqSILModel.h"
#include "pqProxySILModel.h"
#include "pqTreeViewSelectionHelper.h"
#include "pqTreeWidgetSelectionHelper.h"
#include "pqPropertyLinks.h"

#include <QHeaderView>

class pqExtractGroupPanel::pqUI : public QObject, public Ui::ExtractGroupPanel
{
public:
  pqUI(pqExtractGroupPanel* p) :
    QObject(p)
  {
    this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    this->SILUpdateStamp = -1;
  }

  ~pqUI()
  {
  }

  pqSILModel SILModel;
  vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;
  pqPropertyLinks Links;
  pqProxySILModel* entityModel;
  pqProxySILModel* groupModel;
  int SILUpdateStamp;
};

pqExtractGroupPanel::pqExtractGroupPanel(pqProxy* object_proxy, QWidget* p) :
  Superclass(object_proxy, p)
{
  this->UI = new pqUI(this);
  this->UI->setupUi(this);

  this->UI->VTKConnect->Connect(this->proxy(),
      vtkCommand::UpdateInformationEvent, this, SLOT(updateSIL()));
  pqProxySILModel* proxyModel;

  // connect groups to MeshGroupRoot
  proxyModel = new pqProxySILModel("GroupTree", &this->UI->SILModel);
  proxyModel->setSourceModel(&this->UI->SILModel);
  this->UI->Groups->setModel(proxyModel);
  this->UI->Groups->setHeaderHidden(true);

  this->UI->groupModel = new pqProxySILModel("Groups", &this->UI->SILModel);
  this->UI->groupModel->setSourceModel(&this->UI->SILModel);

  // connect cell types to "EntityRoot"
  proxyModel = new pqProxySILModel("CellTypeTree", &this->UI->SILModel);
  proxyModel->setSourceModel(&this->UI->SILModel);
  this->UI->CellTypes->setModel(proxyModel);
  this->UI->CellTypes->setHeaderHidden(true);

  this->UI->entityModel = new pqProxySILModel("CellTypes", &this->UI->SILModel);
  this->UI->entityModel->setSourceModel(&this->UI->SILModel);

  this->updateSIL();

  this->UI->Groups->header()->setStretchLastSection(true);
  this->UI->CellTypes->header()->setStretchLastSection(true);

  this->linkServerManagerProperties();

  QList<pqTreeWidget*> treeWidgets = this->findChildren<pqTreeWidget*> ();
  foreach (pqTreeWidget* tree, treeWidgets)
      {
      new pqTreeWidgetSelectionHelper(tree);
      }

  QList<pqTreeView*> treeViews = this->findChildren<pqTreeView*> ();
  foreach (pqTreeView* tree, treeViews)
      {
      new pqTreeViewSelectionHelper(tree);
      }

  this->connect(this->UI->groupModel, SIGNAL(valuesChanged()), this, SLOT(setModified()));
  this->connect(this->UI->entityModel, SIGNAL(valuesChanged()), this, SLOT(setModified()));

  this->UI->tabWidget->setCurrentIndex(0);
}

pqExtractGroupPanel::~pqExtractGroupPanel()
{
}

void pqExtractGroupPanel::linkServerManagerProperties()
{
  this->UI->Links.addPropertyLink(this->UI->groupModel, "values",
      SIGNAL(valuesChanged()), this->proxy(), this->proxy()->GetProperty(
          "Groups"));

  this->UI->Links.addPropertyLink(this->UI->entityModel, "values",
      SIGNAL(valuesChanged()), this->proxy(), this->proxy()->GetProperty(
          "CellTypes"));

  this->Superclass::linkServerManagerProperties();

}

void pqExtractGroupPanel::updateSIL()
{
  vtkSMExtractGroupProxy* filter = vtkSMExtractGroupProxy::SafeDownCast(
      this->proxy());
  filter->UpdatePropertyInformation(filter->GetProperty("SILUpdateStamp"));

  int stamp = vtkSMPropertyHelper(filter, "SILUpdateStamp").GetAsInt();
  if(stamp != this->UI->SILUpdateStamp)
    {
    this->UI->SILUpdateStamp = stamp;
    vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
    vtkPVSILInformation* info = vtkPVSILInformation::New();
    pm->GatherInformation(filter->GetConnectionID(),
        vtkProcessModule::DATA_SERVER, info, filter->GetID());
    this->UI->SILModel.update(info->GetSIL());

    this->UI->Groups->expandAll();
    this->UI->CellTypes->expandAll();

    info->Delete();
    }
}
