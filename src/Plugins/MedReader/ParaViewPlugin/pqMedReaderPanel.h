#ifndef _pqMedReaderPanel_h
#define _pqMedReaderPanel_h

#include "pqNamedObjectPanel.h"

#include <QTreeWidget>
class pqTreeWidgetItemObject;

class vtkSMProperty;

class PQCOMPONENTS_EXPORT pqMedReaderPanel: public pqNamedObjectPanel
{
Q_OBJECT
  typedef pqNamedObjectPanel Superclass;
public:
  /// constructor
  pqMedReaderPanel(pqProxy* proxy, QWidget* p = NULL);
  /// destructor
  ~pqMedReaderPanel();

public slots:
  // accept changes made by this panel
  //virtual void accept();
  // reset changes made by this panel
  //virtual void reset();

protected slots:
  //void geometryItemChanged(QTreeWidgetItem * item, int column);
  void timeModeChanged(int timeMode);

  void timeComboChanged(int timeStep);

  void updateSIL();

protected:
  /// populate widgets with properties from the server manager
  virtual void linkServerManagerProperties();

  enum PixmapType
  {
    PM_NONE = -1,
    PM_POINT,
    PM_CELL,
    PM_QUADRATURE
  };

  void addSelectionsToTreeWidget(const QString& prop,
      QTreeWidget* tree, PixmapType pix);

  void addSelectionToTreeWidget(const QString& name,
      const QString& realName, QTreeWidget* tree, PixmapType pix,
      const QString& prop, int propIdx);

  void setupTimeModeWidget();
  void updateAvailableTimes();

  class pqUI;
  pqUI* UI;
};

#endif

