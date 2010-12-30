# This case corresponds to: /visu/Plot3D/G2 case
# Create Plot 3D for field of the the given MED file for 10 timestamps%

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
from pvsimple import *
import paravis

# Create presentations
myParavis = paravis.myParavis

picturedir = get_picture_dir(sys.argv[1], "Plot3D/G2")

theFileName = datadir +  "Bug829_resu_mode_236.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "


#print "Import %s..." % theFileName.split('/')[-1],
result = myParavis.ImportFile(theFileName)
aProxy = GetActiveSource()
if aProxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"
# Get viewScalarMap
aView = GetRenderView()

aFieldNames = aProxy.PointArrays.GetData()
aNbOnNodes = len(aFieldNames)
aFieldNames.extend(aProxy.CellArrays.GetData())
aTimeStamps = aProxy.TimestepValues.GetData()
aFieldEntity = EntityType.NODE
aFieldName = "MODES_DEPL"

#create Plot 3D presentations for 10 timestamps
for i in range(1,11):
    hide_all(aView, True)
    aPrs = Plot3DOnField(aProxy, aFieldEntity,aFieldName , i)
    if aPrs is None:
        raise RuntimeError, "Presentation is None!!!"
    #display only current scalar map
    aPrs.Visibility=1
    reset_view(aView)
    Render(aView)    
    
    # Add path separator to the end of picture path if necessery
    if not picturedir.endswith(os.sep):
            picturedir += os.sep
    prs_type = PrsTypeEnum.PLOT3D
            
    # Get name of presentation type
    prs_name = PrsTypeEnum.get_name(prs_type)    
    f_prs_type = prs_name.replace(' ', '').upper()
    # Construct image file name
    pic_name = picturedir + aFieldName + "_" + str(i) + "_" + f_prs_type + "." + pictureext
    
    # Show and record the presentation
    process_prs_for_test(aPrs, aView, pic_name)


