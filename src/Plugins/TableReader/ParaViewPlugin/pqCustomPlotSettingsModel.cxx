#include "pqCustomPlotSettingsModel.h"

#include "vtkSMXYChartRepresentationProxy.h"
#include "pqDataRepresentation.h"
#include "vtkWeakPointer.h"
#include "vtkSMPropertyHelper.h"

#include <QPointer>


class pqCustomPlotSettingsModel::pqImplementation
{
public:
  pqImplementation()
  {
  }

  vtkWeakPointer<vtkSMXYChartRepresentationProxy> RepresentationProxy;
  QPointer<pqDataRepresentation> Representation;
};

pqCustomPlotSettingsModel::pqCustomPlotSettingsModel(QObject* parentObject) :
  Superclass(parentObject), Implementation(new pqImplementation())
{
}

pqCustomPlotSettingsModel::~pqCustomPlotSettingsModel()
{
  delete this->Implementation;
}

//-----------------------------------------------------------------------------
void pqCustomPlotSettingsModel::setRepresentation(pqDataRepresentation* rep)
{
  Superclass::setRepresentation(rep);

  if (!rep || rep == this->Implementation->Representation)
    {
    return;
    }

  if (this->Implementation->Representation)
    {
    QObject::disconnect(this->Implementation->Representation, 0, this, 0);
    }

  this->Implementation->RepresentationProxy =
    vtkSMXYChartRepresentationProxy::SafeDownCast(rep->getProxy());
  this->Implementation->Representation = rep;
}

//-----------------------------------------------------------------------------
pqDataRepresentation* pqCustomPlotSettingsModel::representation() const
{
  return this->Implementation->Representation;
}

//-----------------------------------------------------------------------------
bool pqCustomPlotSettingsModel::setData(const QModelIndex &idx, const QVariant &value,
					int role)
{
  bool result = false;
  if (idx.isValid() && idx.model() == this)
    {
    if (idx.column() == 1 && (role == Qt::DisplayRole || role == Qt::EditRole))
      {
      QString name = value.toString();
      if (!name.isEmpty())
        {
        this->setSeriesLabel(idx.row(), name);
        }
      }
    else if(idx.column() == 0 && role == Qt::CheckStateRole)
      {
      result = true;
      int checkstate = value.toInt();
      this->setSeriesEnabled(idx.row(), checkstate == Qt::Checked);
      }
    }
  return result;
}

//-----------------------------------------------------------------------------
void pqCustomPlotSettingsModel::setSeriesEnabled(int row, bool enabled)
{
  if (row >= 0 && row < this->rowCount(QModelIndex()))
    {
      int minRow = row;
      int maxRow = row;
      
      if (enabled && !(this->IgnoreUnitsModeOn)) 
	{
	  QString unit = getUnit(QString(this->getSeriesName(row)));

	  for (int i = 0; i < rowCount(QModelIndex()); i++)
	    {
	      if (i == row)
		{
		  continue;
		}

	      bool rowChanged = false;
	      QString seriesName = QString(this->getSeriesName(i));
	      QString seriesUnit = getUnit(seriesName);

	      if ((seriesUnit != unit) && getSeriesEnabled(i))
		{
		  vtkSMPropertyHelper(this->Implementation->RepresentationProxy,
		  "SeriesVisibility").SetStatus(this->getSeriesName(i), 0);
		  rowChanged = true;
		}
	      
	      if (!seriesUnit.isEmpty() && this->AutoSelectModeOn &&
		  (seriesUnit == unit) && !getSeriesEnabled(i))
		{
		  vtkSMPropertyHelper(this->Implementation->RepresentationProxy,
		  "SeriesVisibility").SetStatus(this->getSeriesName(i), 1);
		  rowChanged = true;
		}
	      
	      if (rowChanged)
		{
		  if (i < minRow)
		    {
		      minRow = i;
		    }
		  else if (i > maxRow)
		    {
		      maxRow = i;
		    }
		}
	    }
	}
      
      vtkSMPropertyHelper(this->Implementation->RepresentationProxy,
      "SeriesVisibility").SetStatus(this->getSeriesName(row), enabled ? 1 : 0);
      this->Implementation->RepresentationProxy->UpdateVTKObjects();
    
      this->setSeriesColor(row, this->getSeriesColor(row));
      QModelIndex topLeft = this->createIndex(minRow, 0);
      QModelIndex bottomRight = this->createIndex(maxRow, 0);
      emit this->dataChanged(topLeft, bottomRight);
      emit this->redrawChart();
      this->updateCheckState(0, Qt::Horizontal);
    }
}

//-----------------------------------------------------------------------------
void pqCustomPlotSettingsModel::SetIgnoreUnitsModeOn(bool enabled)
{
  this->IgnoreUnitsModeOn = enabled;

  if (this->IgnoreUnitsModeOn)
    {
      this->setCheckable(0, Qt::Horizontal, true);
      this->setCheckState(0, Qt::Horizontal, Qt::Unchecked);
    }
  else
    {
      this->setCheckState(0, Qt::Horizontal, Qt::Unchecked);
      this->setCheckable(0, Qt::Horizontal, false);
    }
}

//-----------------------------------------------------------------------------
void pqCustomPlotSettingsModel::SetAutoSelectModeOn(bool enabled)
{
  this->AutoSelectModeOn = enabled;
  if (enabled && !this->IgnoreUnitsModeOn)
    {
      for (int i = 0; i < rowCount(QModelIndex()); i++)
	{
	  if (getSeriesEnabled(i))
	    {
	      setSeriesEnabled(i, true);
	      break;
	    }
	}
    }
}

//-----------------------------------------------------------------------------
QString pqCustomPlotSettingsModel::getUnit(const QString& seriesName)
{
  QString unit("");
  
  int index1 = seriesName.lastIndexOf("]");
  if (index1 == seriesName.size() - 1)
    {
      int index2 = seriesName.lastIndexOf(" [");
      if (index2 > 0)
	{
	  int start = index2 + 2;
	  unit = seriesName.mid(index2 + 2, index1 - start);
	}
    }

  return unit;
}
