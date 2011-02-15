#ifndef _pqCustomChartDisplayPanelImplementation_h
#define _pqCustomChartDisplayPanelImplementation_h

// #include <QWidget>
// #include <QVariant>
#include "pqDisplayPanelInterface.h"
// #include "pqRepresentation.h"
// #include "vtkSMProxy.h"
// #include "pqDisplayProxyEditor.h"

#include <QObject>

class QWidget;

class pqCustomChartDisplayPanelImplementation : public QObject,
                                                public pqDisplayPanelInterface
{
  Q_OBJECT
  Q_INTERFACES(pqDisplayPanelInterface)

  public:
    // Constructor
    pqCustomChartDisplayPanelImplementation(){}
    pqCustomChartDisplayPanelImplementation(QObject* p);

    // Destructor
    virtual ~pqCustomChartDisplayPanelImplementation(){}

    // Returns true if this panel can be created for the given the proxy.
    virtual bool canCreatePanel(pqRepresentation* proxy) const;

    // Creates a panel for the given proxy
    virtual pqDisplayPanel* createPanel(pqRepresentation* proxy, QWidget* p);
};

#endif

