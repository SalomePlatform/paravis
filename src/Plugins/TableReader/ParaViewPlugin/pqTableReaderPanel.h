#ifndef _pqTableReaderPanel_h
#define _pqTableReaderPanel_h

#include "ui_TableReaderPanel.h"

#include "pqNamedObjectPanel.h"

class QSpinBox;

class pqTableReaderPanel: public pqNamedObjectPanel
{
Q_OBJECT
  typedef pqNamedObjectPanel Superclass;
public:
  // Constructor
  pqTableReaderPanel(pqProxy* proxy, QWidget* p = NULL);

  // Destructor
  ~pqTableReaderPanel();

protected slots:
  void onDelimiterChanged(const QString& value);
  void onCurrentTableChanged(int currentTableIndex);

protected:
  // Populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

  // Update the list with available table names
  void updateAvailableTables(const bool keepCurrent);

  class pqUI;
  pqUI* UI;
};

#endif

