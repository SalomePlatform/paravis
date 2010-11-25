#ifndef _pqExtractGroupPanel_h
#define _pqExtractGroupPanel_h

#include "pqNamedObjectPanel.h"

#include <QTreeWidget>
class pqTreeWidgetItemObject;

class vtkSMProperty;

class PQCOMPONENTS_EXPORT pqExtractGroupPanel: public pqNamedObjectPanel
{
Q_OBJECT
  typedef pqNamedObjectPanel Superclass;
public:
  /// constructor
  pqExtractGroupPanel(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqExtractGroupPanel();

public slots:
  // accept changes made by this panel
  //virtual void accept();
  // reset changes made by this panel
  //virtual void reset();

protected slots:

  void updateSIL();

protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

  class pqUI;
  pqUI* UI;
};

#endif

