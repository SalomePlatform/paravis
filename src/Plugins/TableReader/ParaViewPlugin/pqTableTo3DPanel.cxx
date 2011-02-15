#include "pqTableTo3DPanel.h"

#include <QLineEdit>
#include <QCheckBox>
#include <QComboBox>

pqTableTo3DPanel::pqTableTo3DPanel(pqProxy* proxy, QWidget* p) :
  Superclass(proxy, p)
{
  QLineEdit* scaleFactor = this->findChild<QLineEdit*>("ScaleFactor");
  QCheckBox* useOptimusScale = this->findChild<QCheckBox*>("UseOptimusScale");
  QComboBox* presentationType = this->findChild<QComboBox*>("PresentationType");
  QWidget* numberContours = this->findChild<QWidget*>("NumberOfContours");
  
  if (scaleFactor && useOptimusScale)
    {
      this->connect(useOptimusScale, SIGNAL(toggled(bool)),
		    this, SLOT(onScaleModeChanged(bool)));
      onScaleModeChanged(useOptimusScale->isChecked());
    }

  if (presentationType && numberContours)
    {
      this->connect(presentationType, SIGNAL(currentIndexChanged(const QString &)),
		    this, SLOT(onPrsTypeChanged(const QString &)));
      onPrsTypeChanged(presentationType->currentText());
    }
}

pqTableTo3DPanel::~pqTableTo3DPanel()
{
}

void pqTableTo3DPanel::onScaleModeChanged(bool checked)
{
  QLineEdit* scaleFactor = this->findChild<QLineEdit*>("ScaleFactor");
  if (scaleFactor)
    {
      scaleFactor->setEnabled(!checked);
    }
}

void pqTableTo3DPanel::onPrsTypeChanged(const QString& type)
{
  QWidget* numberContours = this->findChild<QWidget*>("NumberOfContours");
  if (numberContours)
    {
      numberContours->setEnabled(type == "Contour");
    }
}
