# This case corresponds to: /visu/SWIG_scripts/A5 case
# Create table and display it as curves

from paravistest import tablesdir
import paravis
import pvsimple


# Define script for table creation
script = """
import math


# Get table output
table = self.GetTableOutput()

nb_rows = 10
nb_cols = 200

# Create first column
col1 = vtk.vtkDoubleArray()
col1.SetName('Frequency')
for i in xrange(0, nb_rows):
    col1.InsertNextValue(i * 10 + 1)
table.AddColumn(col1)

# Create the rest columns
for i in xrange(1, nb_cols + 1):
   col = vtk.vtkDoubleArray()
   col.SetName('Power ' + str(i))

   # Fill the next column
   for j in xrange(0, nb_rows):
      if j % 2 == 1:
         col.InsertNextValue(math.log10(j * 30 * math.pi / 180) * 20 + i * 15 + j * 5)
      else:
         col.InsertNextValue(math.sin(j * 30 * math.pi / 180) * 20 + i * 15 + j * 5)

   table.AddColumn(col)
"""

# Creating programmable source (table)
ps = pvsimple.ProgrammableSource()
ps.OutputDataSetType = 'vtkTable'
ps.Script = script
pvsimple.RenameSource("Very useful data", ps)
ps.UpdatePipeline()

# Display curves
cur_view = pvsimple.GetRenderView()
if cur_view:
    pvsimple.Delete(cur_view)
xy_view = pvsimple.CreateXYPlotView()
xy_view.ChartTitle = 'Very useful data'
xy_view.AxisTitle = ['[ Wt ]', 'Frequency [ Hz ]']

xy_rep = pvsimple.Show(ps)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
xy_rep.XArrayName = 'Frequency'
xy_rep.SeriesVisibility = ['Frequency', '0']

pvsimple.Render()
