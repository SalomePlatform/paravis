# Copyright (C) 2010-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

# This case corresponds to: /visu/SWIG_scripts/B7 case
# Import a table from file and show it in 2D viewer

from paravistest import tablesdir
from presentations import *
import pvsimple

# Import table from CSV file
file_path = tablesdir + "table_test.csv"
table_csv = pvsimple.CSVReader(FileName=file_path)

# Set space as separator
table_csv.FieldDelimiterCharacters = ';'

# Display curve
cur_view = pvsimple.GetRenderView()
if cur_view:
    delete_pv_object(cur_view)
xy_view = pvsimple.CreateXYPlotView()

xy_rep = pvsimple.Show(table_csv)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
xy_rep.XArrayName = 'X'
xy_rep.SeriesVisibility = ['X', '0']

# Set view properties
title = xy_view.ChartTitle
xy_view.ChartTitle = "Change the title from python"
pvsimple.Render(xy_view)

xy_view.LeftAxisTitle = "Y axis"
xy_view.BottomAxisTitle = "X axis"
pvsimple.Render(xy_view)

xy_view.ShowLegend = 0
pvsimple.Render(xy_view)

xy_view.ShowLeftAxisGrid = 0
xy_view.ShowLeftAxisLabels = 0
xy_view.ShowBottomAxisGrid = 0
xy_view.ShowBottomAxisLabels = 0
xy_view.ShowRightAxisGrid = 0
xy_view.ShowRightAxisLabels = 0
xy_view.ShowTopAxisGrid = 0
xy_view.ShowTopAxisLabels = 0
pvsimple.Render(xy_view)
xy_view.ShowLeftAxisGrid = 1
xy_view.ShowLeftAxisLabels = 1
pvsimple.Render(xy_view)
xy_view.ShowBottomAxisGrid = 1
xy_view.ShowBottomAxisLabels = 1
pvsimple.Render(xy_view)

# Set logarithmic scaling
xy_view.LeftAxisLogScale = 1
pvsimple.Render(xy_view)
xy_view.BottomAxisLogScale = 1
pvsimple.Render(xy_view)

# Set representation properties

# Show/hide curves
xy_rep.SeriesVisibility = ['Y0', '0', 'Y1', '0',
                           'Y2', '0', 'Y3', '0']
pvsimple.Render(xy_view)
xy_rep.SeriesVisibility = ['Y0', '1']
pvsimple.Render(xy_view)
xy_rep.SeriesVisibility = ['Y1', '1']
pvsimple.Render(xy_view)
xy_rep.SeriesVisibility = ['Y2', '1']
pvsimple.Render(xy_view)

xy_rep.SeriesVisibility = ['Y3', '1']
xy_rep.SeriesVisibility = ['Y3', '0']
xy_rep.SeriesVisibility = ['Y3', '1']
pvsimple.Render(xy_view)

# Line style
xy_rep.SeriesLineStyle = ['Y0', '5']
pvsimple.Render(xy_view)
xy_rep.SeriesLineStyle = ['Y1', '4']
pvsimple.Render(xy_view)
xy_rep.SeriesLineStyle = ['Y2', '3']
pvsimple.Render(xy_view)
xy_rep.SeriesLineStyle = ['Y3', '2']
pvsimple.Render(xy_view)

# Line thickness
values = (2, 4, 6, 8, 10, 1)
for val in values:
    xy_rep.SeriesLineThickness = ['Y3', str(val)]
    pvsimple.Render(xy_view)

xy_rep.SeriesLineStyle = ['Y3', '0']
pvsimple.Render(xy_view)

# Marker type
xy_rep.SeriesMarkerStyle = ['Y3', '5']
pvsimple.Render(xy_view)
xy_rep.SeriesMarkerStyle = ['Y2', '4']
pvsimple.Render(xy_view)
xy_rep.SeriesMarkerStyle = ['Y1', '3']
pvsimple.Render(xy_view)
xy_rep.SeriesMarkerStyle = ['Y0', '8']
pvsimple.Render(xy_view)

# Marker size
for val in values:
    xy_rep.SeriesLineThickness = ['Y3', str(val)]
    pvsimple.Render(xy_view)
