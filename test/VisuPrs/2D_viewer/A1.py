# This case corresponds to: /visu/2D_viewer/A1 case

from paravistest import test_values
from pvsimple import CreateXYPlotView

# Create view
aXYPlot = CreateXYPlotView()

# Set range
axisRange = aXYPlot.AxisRange
# Left axis range
axisRange[0:2] = [200., 800.]
Ranges = aXYPlot.AxisRange
Etvalue = [200, 800, axisRange[2], axisRange[3],
           axisRange[4], axisRange[5], axisRange[6], axisRange[7] ]
error = test_values(Ranges, Etvalue)
# Bottom axis range
aXYPlot.AxisRange[2:4] = [350.,750.]
Ranges = aXYPlot.AxisRange
Etvalue = [200, 800, 350, 750,
           axisRange[4], axisRange[5], axisRange[6], axisRange[7]]
error = error + test_values(Ranges, Etvalue)
# Left and bottom axis range
aXYPlot.AxisRange[0:4] = [1350.,2750.,240.,2230.]
Ranges=aXYPlot.AxisRange
Etvalue=[1350.,2750.,240.,2230.,
         axisRange[4], axisRange[5], axisRange[6], axisRange[7]]
error = error + test_values(Ranges, Etvalue)
# Left, bottom, right and top axis range
aXYPlot.AxisRange = [0, 200, 100, 450, 0, 200, 100, 450]
Ranges = aXYPlot.AxisRange
Etvalue = [0, 200, 100, 450, 0, 200, 100, 450]
error = error + test_values(Ranges, Etvalue)

if error > 0:
    raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
