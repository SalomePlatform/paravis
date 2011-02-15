#ifndef __pqCustomPlotSettingsModel_h
#define __pqCustomPlotSettingsModel_h

#include "pqPlotSettingsModel.h"

class pqDataRepresentation;

class pqCustomPlotSettingsModel : public pqPlotSettingsModel
{
  typedef pqPlotSettingsModel Superclass;

  Q_OBJECT

public:
  pqCustomPlotSettingsModel(QObject* parent = 0);
  ~pqCustomPlotSettingsModel();

  void setRepresentation(pqDataRepresentation* rep);
  pqDataRepresentation* representation() const;

  /// \brief Sets the data for the given model index.
  /// \param index The model index.
  /// \param value The new data for the given role.
  /// \param role The role to set data for.
  /// \return True if the data was changed successfully.
  virtual bool setData(const QModelIndex &index, const QVariant &value,
                       int role=Qt::EditRole);

  void SetIgnoreUnitsModeOn(bool enabled);
  void SetAutoSelectModeOn(bool enabled);

public slots:

  // Description:
  // API to enable series.
  void setSeriesEnabled(int row, bool enabled);

private:
  QString getUnit(const QString& seriesName); 
  
private:
  class pqImplementation;
  pqImplementation* Implementation;

  bool IgnoreUnitsModeOn;
  bool AutoSelectModeOn;
};

#endif
