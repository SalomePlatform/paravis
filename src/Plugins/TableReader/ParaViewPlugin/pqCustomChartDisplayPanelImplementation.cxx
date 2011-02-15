#include "pqCustomChartDisplayPanelImplementation.h"
#include "pqCustomXYChartDisplayPanel.h"

#include "vtkSMProxy.h"

#include "pqDataRepresentation.h"
#include "pqPipelineSource.h"


pqCustomChartDisplayPanelImplementation::pqCustomChartDisplayPanelImplementation(QObject* p):
QObject(p)
    {
    }

bool pqCustomChartDisplayPanelImplementation::canCreatePanel(pqRepresentation* repr) const
{
  if(!repr || !repr->getProxy() || 
     (repr->getProxy()->GetXMLName() != QString("XYChartRepresentation")))
    {
      return false;
    }

  pqDataRepresentation* dataRepr = qobject_cast<pqDataRepresentation*>(repr);
  if(dataRepr)
    {
      pqPipelineSource* input = dataRepr->getInput(); 
      QString name = input->getProxy()->GetXMLName();
      if (name == "TableReader")
	{
	  return true;
	}
    }
  
  return false;
}

pqDisplayPanel* pqCustomChartDisplayPanelImplementation::createPanel(pqRepresentation* repr, QWidget* p)
{
  if(!repr || !repr->getProxy() ||
     (repr->getProxy()->GetXMLName() != QString("XYChartRepresentation")))
    {
      return NULL;
    }

  pqDataRepresentation* dataRepr = qobject_cast<pqDataRepresentation*>(repr);
  if(dataRepr)
    {
      pqPipelineSource* input = dataRepr->getInput(); 
      QString name=input->getProxy()->GetXMLName();

      if (name == "TableReader")
	{
	  return new pqCustomXYChartDisplayPanel(repr, p);
	}
    }
  
  return NULL;
}


