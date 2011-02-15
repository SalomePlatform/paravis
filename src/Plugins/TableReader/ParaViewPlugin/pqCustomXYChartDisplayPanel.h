#ifndef __pqCustomXYChartDisplayPanel_h
#define __pqCustomXYChartDisplayPanel_h

#include "pqDisplayPanel.h"

class pqRepresentation;
class pqDataInformationModel;
class QModelIndex;

/// Editor widget for XY chart displays.
class pqCustomXYChartDisplayPanel : public pqDisplayPanel
{
  Q_OBJECT
public:
  pqCustomXYChartDisplayPanel(pqRepresentation* display, QWidget* parent=0);
  virtual ~pqCustomXYChartDisplayPanel();

public slots:
  /// Reloads the series list from the display.
  void reloadSeries();

protected slots:
  /// Slot to listen to clicks for changing color.
  void activateItem(const QModelIndex &index);

  void updateOptionsWidgets();

  void setCurrentSeriesColor(const QColor &color);

  void setCurrentSeriesThickness(int thickness);

  void setCurrentSeriesStyle(int listIndex);

  void setCurrentSeriesAxes(int listIndex);

  void setCurrentSeriesMarkerStyle(int listIndex);

  void useArrayIndexToggled(bool);

  void useDataArrayToggled(bool);

  /// Unit controls
  void autoSelectToggled(bool checked);
  void ignoreUnitsToggled(bool checked);

  /// Reset state of the controls relative to units
  void resetUnitsControls();

  /// Update view options
  void updateViewOptions();

private:
  pqCustomXYChartDisplayPanel(const pqCustomXYChartDisplayPanel&); // Not implemented.
  void operator=(const pqCustomXYChartDisplayPanel&); // Not implemented.

  /// Set the display whose properties this editor is editing.
  /// This call will raise an error if the display is not
  /// an XYChartRepresentation proxy.
  void setDisplay(pqRepresentation* display);

  /// Disable/enable elements of the dialog based on the chart type.
  void changeDialog(pqRepresentation* display);
  
  Qt::CheckState getEnabledState() const;

  class pqInternal;
  pqInternal* Internal;

  pqDataInformationModel* Model;
};

#endif
