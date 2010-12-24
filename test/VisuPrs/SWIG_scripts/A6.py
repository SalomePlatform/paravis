# This case corresponds to: /visu/SWIG_scripts/A6 case
# Import a table from file and show it in 2D viewer

from paravistest import tablesdir
from presentations import *
import paravis
import pvsimple


# Import table from CSV file
file_path = tablesdir + "sinus.csv"
sinus_csv = pvsimple.CSVReader(FileName=file_path)

# Import settings
sinus_csv.DetectNumericColumns = 1
sinus_csv.UseStringDelimiter = 1
sinus_csv.FieldDelimiterCharacters = ' '
sinus_csv.HaveHeaders = 1

# Display curve
cur_view = pvsimple.GetRenderView()
if cur_view:
    pvsimple.Delete(cur_view)
xy_view = pvsimple.CreateXYPlotView()

xy_rep = pvsimple.Show(sinus_csv)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
xy_rep.XArrayName = 'x'
xy_rep.SeriesVisibility = ['x', '0']

pvsimple.Render()
