#include "pqTableReaderPanel.h"

#include "vtkSMProxy.h"
#include "vtkSMStringVectorProperty.h"
#include "vtkProcessModule.h"

#include "pqPropertyLinks.h"

#include <QLayout>
#include <QSpinBox>


class pqTableReaderPanel::pqUI: public QObject, public Ui::TableReaderPanel
{
public:
  pqUI(pqTableReaderPanel* p) : QObject(p)
  {
  }

  ~pqUI()
  {
  }

  pqPropertyLinks Links;
};


pqTableReaderPanel::pqTableReaderPanel(pqProxy* proxy, QWidget* p) :
  Superclass(proxy, p)
{
  this->UI = new pqUI(this);
  this->UI->setupUi(this);
  
  this->linkServerManagerProperties();
  this->updateAvailableTables(false);

  this->connect(this->UI->ValueDelimiter, SIGNAL(textChanged(const QString&)),
		this, SLOT(onDelimiterChanged(const QString&)));
  this->connect(this->UI->TableNames, SIGNAL(currentIndexChanged(int)),
		this, SLOT(onCurrentTableChanged(int)));
}

pqTableReaderPanel::~pqTableReaderPanel()
{
}

void pqTableReaderPanel::onCurrentTableChanged(int currentTableIndex)
{
  this->setModified();
}

void pqTableReaderPanel::onDelimiterChanged(const QString& value)
{
  this->updateAvailableTables(true);
}

void pqTableReaderPanel::linkServerManagerProperties()
{
  this->UI->Links.addPropertyLink(this->UI->TableNames, "currentIndex",
	  SIGNAL(currentIndexChanged(int)), this->proxy(), 
	  this->proxy()->GetProperty("TableNumber"));

  // To hook up the rest widgets
  this->Superclass::linkServerManagerProperties();
}

void pqTableReaderPanel::updateAvailableTables(const bool keepCurrent)
{
  vtkSMStringVectorProperty* prop = vtkSMStringVectorProperty::SafeDownCast(
				 this->proxy()->GetProperty("AvailableTables"));
  
  int currentIndex = this->UI->TableNames->currentIndex();
  QString currentText = this->UI->TableNames->currentText();

  this->UI->TableNames->clear();
  
  for(int id = 0; id < prop->GetNumberOfElements(); id++) 
    {
      QString text(prop->GetElement(id));
      if (text.isEmpty()) 
	{
	  text = QString("Table:%1").arg(id);
	}
      this->UI->TableNames->addItem(text);
    }

  if (keepCurrent && 
      currentIndex < this->UI->TableNames->maxCount() &&
      currentText == this->UI->TableNames->itemText(currentIndex)) 
    {
      this->UI->TableNames->setCurrentIndex(currentIndex);
    }
}
