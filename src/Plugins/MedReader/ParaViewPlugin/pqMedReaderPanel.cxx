#include "pqMedReaderPanel.h"
#include "ui_MedReaderPanel.h"

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
#include "vtkSMMedReaderProxy.h"

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

class pqMedReaderPanel::pqUI: public QObject, public Ui::MedReaderPanel
{
public:
  pqUI(pqMedReaderPanel* p) :
    QObject(p)
  {
    this->VTKConnect = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    this->SILUpdateStamp = -1;
    this->Links.setAutoUpdateVTKObjects(false);
  }

  ~pqUI()
  {
  }

  pqSILModel SILModel;
  vtkSmartPointer<vtkEventQtSlotConnect> VTKConnect;
  pqPropertyLinks Links;
  QMap<QTreeWidgetItem*, QString> TreeItemToPropMap;
  pqProxySILModel* entityModel;
  pqProxySILModel* groupModel;
  int SILUpdateStamp;
};

pqMedReaderPanel::pqMedReaderPanel(pqProxy* object_proxy, QWidget* p) :
  Superclass(object_proxy, p)
{
  this->UI = new pqUI(this);
  this->UI->setupUi(this);

  pqProxySILModel* proxyModel;

  // connect groups to groupsRoot
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

  this->UI->VTKConnect->Connect(this->proxy(),
      vtkCommand::UpdateInformationEvent, this, SLOT(updateSIL()));
}

pqMedReaderPanel::~pqMedReaderPanel()
{
}

void pqMedReaderPanel::addSelectionsToTreeWidget(const QString& prop,
    QTreeWidget* tree, PixmapType pix)
{
  vtkSMProperty* SMProperty = this->proxy()->GetProperty(prop.toAscii().data());
  QList<QVariant> SMPropertyDomain;
  SMPropertyDomain = pqSMAdaptor::getSelectionPropertyDomain(SMProperty);
  int j;
  for(j = 0; j < SMPropertyDomain.size(); j++)
    {
    QString varName = SMPropertyDomain[j].toString();
    this->addSelectionToTreeWidget(varName, varName, tree, pix, prop, j);
    }
}

void pqMedReaderPanel::addSelectionToTreeWidget(const QString& name,
    const QString& realName, QTreeWidget* tree, PixmapType pix,
    const QString& prop, int propIdx)
{
  static QPixmap pixmaps[] = { QPixmap(
      ":/ParaViewResources/Icons/pqPointData16.png"), QPixmap(
      ":/ParaViewResources/Icons/pqCellData16.png"), QPixmap(
      ":/ParaViewResources/Icons/pqQuadratureData16.png") };

  vtkSMProperty* SMProperty = this->proxy()->GetProperty(prop.toAscii().data());

  if(!SMProperty || !tree)
    {
    return;
    }

  QList<QString> strs;
  strs.append(name);
  pqTreeWidgetItemObject* item;
  item = new pqTreeWidgetItemObject(tree, strs);
  item->setData(0, Qt::ToolTipRole, name);
  if(pix >= 0)
    {
    item->setData(0, Qt::DecorationRole, pixmaps[pix]);
    }
  item->setData(0, Qt::UserRole, QString("%1 %2").arg((int) pix).arg(realName));
  this->propertyManager()->registerLink(item, "checked",
      SIGNAL(checkedStateChanged(bool)), this->proxy(), SMProperty, propIdx);

  this->UI->TreeItemToPropMap[item] = prop;
}

void pqMedReaderPanel::linkServerManagerProperties()
{
  this->UI->Links.addPropertyLink(this->UI->groupModel, "values",
      SIGNAL(valuesChanged()), this->proxy(), this->proxy()->GetProperty(
          "Groups"));

  this->UI->Links.addPropertyLink(this->UI->entityModel, "values",
      SIGNAL(valuesChanged()), this->proxy(), this->proxy()->GetProperty(
          "CellTypes"));

  this->Superclass::linkServerManagerProperties();

  // do the point variables
  this->addSelectionsToTreeWidget("PointFieldsArrayStatus",
      this->UI->Variables, PM_POINT);
  // do the cell variables
  this->addSelectionsToTreeWidget("CellFieldsArrayStatus", this->UI->Variables,
      PM_CELL);
  // do the quadrature variables
  this->addSelectionsToTreeWidget("QuadratureFieldsArrayStatus",
      this->UI->Variables, PM_QUADRATURE);

  this->setupTimeModeWidget();
}

void pqMedReaderPanel::setupTimeModeWidget()
{
  this->UI->AnimationModeCombo->clear();
  QList<QVariant> modes = pqSMAdaptor::getEnumerationPropertyDomain(
      this->proxy()->GetProperty("AnimationMode"));
  for(int mode = 0; mode < modes.size(); mode++)
    {
    QString modeName = modes[mode].toString();
    this->UI->AnimationModeCombo->addItem(modeName);
    }

  this->UI->Links.addPropertyLink(this->UI->AnimationModeCombo, "currentIndex",
      SIGNAL(currentIndexChanged(int)), this->proxy(),
      this->proxy()->GetProperty("AnimationMode"));

  this->connect(this->UI->AnimationModeCombo, SIGNAL(currentIndexChanged(int)),
      this, SLOT(timeModeChanged(int)));

  timeModeChanged(0);

  this->connect(this->UI->TimeCombo, SIGNAL(currentIndexChanged(int)), this,
      SLOT(timeComboChanged(int)));

  timeComboChanged(0);
}

void pqMedReaderPanel::timeComboChanged(int timeStep)
{
  int timeMode = this->UI->AnimationModeCombo->currentIndex();
  double timeValue;

  if (( timeMode == 2 ) || ( timeMode == 3 ))
  {
  vtkSMDoubleVectorProperty* prop = vtkSMDoubleVectorProperty::SafeDownCast(
      this->proxy()->GetProperty("AvailableTimes"));

  if (( timeStep > prop->GetNumberOfElements()/2 ) || ( timeStep < 0 ))
  {
      timeValue = prop->GetElement(0);
  }
  else
  {
      timeValue = prop->GetElement(2*timeStep);
  }

  vtkSMDoubleVectorProperty::SafeDownCast(
              this->proxy()->GetProperty("Time"))->SetElements1(timeValue);

    this->proxy()->UpdateVTKObjects();
    this->setModified();
  }

}

void pqMedReaderPanel::timeModeChanged(int timeMode)
{

  // 0: default / 1: physical time / 2 : iteration / 3 : modes
  vtkSMIntVectorProperty::SafeDownCast(this->proxy()->GetProperty(
      "AnimationMode"))->SetElements1(timeMode);
  if(timeMode == 0 || timeMode == 1)
    {
    this->UI->TimeLabel->setText(tr("Time"));
    this->UI->TimeCombo->hide();
    this->UI->TimeLabel->hide();
    }
  else if(timeMode == 2)
    {
    this->UI->TimeLabel->setText(tr("Time"));
    this->updateAvailableTimes();
    this->UI->TimeCombo->show();
    this->UI->TimeLabel->show();
    }
  else
    {
    this->UI->TimeLabel->setText(tr("Frequency"));
    this->updateAvailableTimes();
    this->UI->TimeCombo->show();
    this->UI->TimeLabel->show();
    }
  this->setModified();
}

void pqMedReaderPanel::updateAvailableTimes()
{
  vtkSMDoubleVectorProperty* prop = vtkSMDoubleVectorProperty::SafeDownCast(
      this->proxy()->GetProperty("AvailableTimes"));

  prop->GetInformationHelper()->UpdateProperty( vtkProcessModuleConnectionManager::GetRootServerConnectionID(),
      vtkProcessModule::DATA_SERVER,
      this->proxy()->GetID(),
      prop);

  int timeMode = this->UI->AnimationModeCombo->currentIndex();
  this->UI->TimeCombo->clear();
  double *aux = prop->GetElements();

  for(int tid = 0; tid < prop->GetNumberOfElements()/2; tid++)
    if (timeMode == 3)
      this->UI->TimeCombo->addItem( "[" + QString::number(aux[2*tid+1]) + "] " + QString::number(aux[2*tid]) );
    else
      this->UI->TimeCombo->addItem( QString::number(aux[2*tid]) );

}

void pqMedReaderPanel::updateSIL()
{
  vtkSMMedReaderProxy* reader = vtkSMMedReaderProxy::SafeDownCast(
      this->proxy());
  reader->UpdatePropertyInformation(reader->GetProperty("SILUpdateStamp"));

  int stamp = vtkSMPropertyHelper(reader, "SILUpdateStamp").GetAsInt();
  if(stamp != this->UI->SILUpdateStamp)
    {
    this->UI->SILUpdateStamp = stamp;
    vtkProcessModule* pm = vtkProcessModule::GetProcessModule();
    vtkPVSILInformation* info = vtkPVSILInformation::New();
    pm->GatherInformation(reader->GetConnectionID(),
        vtkProcessModule::DATA_SERVER, info, reader->GetID());
    this->UI->SILModel.update(info->GetSIL());

    this->UI->Groups->expandAll();
    this->UI->CellTypes->expandAll();

    info->Delete();
    }
}
