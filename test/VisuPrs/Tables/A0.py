# This case corresponds to: /visu/tables/A0 case
# Import table in Post-Pro specific format from the file;
# choose 'sinus' table; create curve.

import sys

from paravistest import tablesdir, get_picture_dir, pictureext
from presentations import *
import paravis
import pvsimple


# Import table from file
print 'Import tables_test.xls ....',
file_path = tablesdir + "tables_test.xls"
table_reader = pvsimple.TableReader(FileName=file_path)
if table_reader is None:
    print "FAILED"
else:
    print "OK"

# Get available tables
print 'Get available tables .....'
available_tables = table_reader.GetPropertyValue("AvailableTables")
if (available_tables is None) or (len(available_tables) == 0):
    print "FAILED"
else:
    print available_tables

# Choose 'sinus' table
table_reader.TableNumber = available_tables.GetData().index('sinus')
table_reader.UpdatePipeline()

# Create curves
cur_view = pvsimple.GetRenderView()
if cur_view:
    pvsimple.Delete(cur_view)
xy_view = pvsimple.CreateXYPlotView()

# xy_rep = pvsimple.Show(table_reader)
xy_rep = pvsimple.Show(table_reader)
xy_rep.AttributeType = 'Row Data'
xy_rep.UseIndexForXAxis = 0
x_array = xy_rep.GetPropertyValue("SeriesNamesInfo")[0]
xy_rep.XArrayName = x_array
xy_rep.SeriesVisibility = [x_array, '0']
pvsimple.Render(xy_view)

# Write image

# Directory for saving snapshots
picturedir = get_picture_dir(sys.argv[1], "Tables/A0")
if not picturedir.endswith(os.sep):
    picturedir += os.sep

file_name = picturedir + "sinus." + pictureext
pvsimple.WriteImage(file_name, view=xy_view, Magnification=1)


