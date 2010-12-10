#This case corresponds to: /visu/Vectors/F9 case
#%Create Vectors for field of the the given MED file for 10 timestamps%

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
from pvsimple import *
import paravis

# Create presentations
myParavis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "Vectors/F9")

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
# Get view
aView = GetRenderView()

# Create required presentations for the proxy
# CreatePrsForProxy(aProxy, aView, thePrsTypeList, thePictureDir, thePictureExt, theIsAutoDelete)
aFieldNames = aProxy.PointArrays.GetData()
aNbOnNodes = len(aFieldNames)
aFieldNames.extend(aProxy.CellArrays.GetData())
aTimeStamps = aProxy.TimestepValues.GetData()
aFieldEntity = 'POINT_DATA'
aFieldName = "MODES_DEPL"

#Creation of a set of non-colored Vectors presentations, based on time stamps of MODES___DEP field

for i in range(1,11):
    HideAll(aView, True)
    aPrs = VectorsOnField(aProxy, aFieldEntity,aFieldName , i)	
    if aPrs is None:
        raise RuntimeError, "Presentation is None!!!"
    # display only current deformed shape
    #DisplayOnly(aView,aPrs)
    Prs.Visibility =1	
    ResetView(aView)
    Render(aView)
    picture_name = picturedir + "time_stamp_1_"+str(i)+"."+pictureext
    # Show and record the presentation
    process_prs_for_test(aPrs, aView, picture_name)
    
#^Creation of a set of colored Vectors presentations, based on time stamps of MODES___DEPL field
aView = CreateRenderView()
#ResetView(aView)
#Render(aView)
for j in range(1,11):
    HideAll(aView, True)
    aPrs2 = VectorsOnField(aProxy, aFieldEntity,aFieldName , j,theIsColored=True)	
    if aPrs2 is None:
        raise RuntimeError, "Presentation is None!!!"
    # display only current deformed shap
    #DisplayOnly(aView,aPrs2)
    aPrs2.Visibility =1	
    ResetView(aView)
    Render(aView)
    picture_name = picturedir + "time_stamp_2_"+str(j)+"."+pictureext
    # Show and record the presentation
    process_prs_for_test(aPrs2, aView, picture_name)
