#This case corresponds to: /visu/DeformedShape/F9 case
#%Create Deformed Shape for field of the the given MED file for 10 timestamps%

from paravistest import * 
from presentations import *
import paravis

# Create presentations
myParavis = paravis.myParavis

theFileName = datadir +  "Bug829_resu_mode.med"
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
#create list to store picture files sizes
sizes=[]

picturedir += "DeformedShape/G2/"

for i in range(1,11):

	aPrs = DeformedShapeOnField(aProxy, aFieldEntity,aFieldName , i)	
	
	if aPrs is None : 
        	raise RuntimeError, "Presentation is None!!!"
	
	# display only current deformed shape
    	DisplayOnly(aView,aPrs)
    	#aView.FitAll()
	
    	picture_name = picturedir + "time_stamp_"+str(i)+"."+pictureext
	
    	# Show and record the presentation
        ProcessPrsForTest(aPrs, aView, picture_name)

    	sizes.append(os.path.getsize(picture_name))
	
# check sizes of pictures	
if abs(max(sizes)-min(sizes)) > 0.01*max(sizes):
    print "<b>WARNING!!! Pictures have different sizes!!!</b>"; errors+=1
    for i in range(1,11):
	picture_name = "time_stamp_"+str(i)+"."+pictureext
	print "Picture: "+picture_name+"; size: "+str(sizes[i-1]) 
    raise RuntimeError	