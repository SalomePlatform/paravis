# This case corresponds to: /visu/2D_viewer/A0 case

import paravistest

from pvsimple import CreateXYPlotView, Delete

# Create view
aXYPlot = CreateXYPlotView()
# Delete view
Delete(aXYPlot)
# Create view again
aXYPlot = CreateXYPlotView()

# Set title
print "Default chart title    ...", aXYPlot.ChartTitle
aXYPlot.ChartTitle = '   '
aXYPlot.ChartTitle = 'title of XY plot'
print "Chart title for xyplot ...", aXYPlot.ChartTitle

# Show left and bottom axis
aXYPlot.ShowAxis = [1, 1, 0, 0]

# Show grids
aXYPlot.ShowAxisGrid = [1, 1, 0, 0]

# Set axis range for left and bottom axis
aXYPlot.AxisBehavior = [1, 1, 0, 0]

aMinX = 0
aMaxX = 100
aMinY = 0
aMaxY = 10
aXYPlot.AxisRange = [aMinX, aMaxX, aMinY, aMaxY, 0.0, 1.0, 0.0, 1.0]

# Set horizontal scaling for left and bottom axis
print "Default logarithmic scaling  ...", aXYPlot.AxisLogScale
# Set logarithmic scaling
aXYPlot.AxisLogScale = [1, 1, 0, 0]
print "Logarithmic scaling          ...", aXYPlot.AxisLogScale
# Set linear scaling
aXYPlot.AxisLogScale = [0, 0, 0, 0]
print "Linear scaling               ...", aXYPlot.AxisLogScale

# Axis titles
print "Default title of the left axis    ...", aXYPlot.AxisTitle[0]
aXYPlot.AxisTitle[0] = '    '
aXYPlot.AxisTitle[0] ="Xtitle of XY plot"
print "Title of the left axis            ...", aXYPlot.AxisTitle[0]
print "Default title of the bottom axis  ...", aXYPlot.AxisTitle[1]
aXYPlot.AxisTitle[1] = '    '
aXYPlot.AxisTitle[1] = "Ytitle of XY plot"
print "Title of the bottom axis          ...", aXYPlot.AxisTitle[1]

# Show/hide legend
aXYPlot.ShowLegend = 1
aXYPlot.ShowLegend = 0
