#This case corresponds to: /visu/IsoSurface/G2 case
#%Create Iso surfaces for field of the the given MED file for 10 timestamps%

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
from pvsimple import *
import paravis

# Create presentations
myParavis = paravis.myParavis
picturedir = get_picture_dir(sys.argv[1], "IsoSurface/G2")

theFileName = datadir +  "Bug829_resu_mode_236.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "

"""Build presentations of the given types for all fields of the given file."""
#print "Import %s..." % theFileName.split('/')[-1],
result = myParavis.ImportFile(theFileName)
aProxy = GetActiveSource()
if aProxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"
# Get viewScalarMap
aView = GetRenderView()

# Create required presentations for the proxy
# CreatePrsForProxy(aProxy, aView, thePrsTypeList, thePictureDir, thePictureExt, theIsAutoDelete)
aFieldNames = aProxy.PointArrays.GetData()
aNbOnNodes = len(aFieldNames)
aFieldNames.extend(aProxy.CellArrays.GetData())
aTimeStamps = aProxy.TimestepValues.GetData()
aFieldEntity = 'POINT_DATA'
aFieldName = "MODES_DEPL"


for i in range(1,11):
    aPrs = IsoSurfacesOnField(aProxy, aFieldEntity,aFieldName , i)
    if aPrs is None:
        raise RuntimeError, "Presentation is None!!!"
    #display only current scalar map
    DisplayOnly(aView,aPrs)
    #aView.FitAll()
    picture_name = picturedir + "time_stamp_"+str(i)+"."+pictureext
    # Show and record the presentation
    process_prs_for_test(aPrs, aView, picture_name)


