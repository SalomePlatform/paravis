#This  case corresponds to: /visu/MeshPresentation/H6 case
# Create  mesh presentation for nodes and cells of the the given MED file 

import sys
from paravistest import datadir, pictureext, get_picture_dir
from presentations import *
from pvsimple import *
import paravis

# Create presentations
myParavis = paravis.myParavis
picturedir = get_picture_dir(sys.argv[1], "MeshPresentation/H6")

theFileName = datadir +  "CornerTetra.med"
print " --------------------------------- "
print "file ", theFileName
print " --------------------------------- "


result = myParavis.ImportFile(theFileName)
aProxy = GetActiveSource()
if aProxy is None:
	raise RuntimeError, "Error: can't import file."
else: print "OK"

aView = GetRenderView()

#%Creation of the mesh presentation%
mesh_name = "CornerTetra"

#^Presentation on "onNodes" and '"onCells" family^
entity_types = [EntityType.NODE,EntityType.CELL]
for entity_type in entity_types:
    entity_name = EntityType.get_name(entity_type)
    mesh = MeshOnEntity(aProxy, mesh_name ,entity_type)
    if mesh is None: 
        msg = "ERROR!!!Presentation of mesh on '"+entity_name+"' family wasn't created..."
        raise RuntimeError, msg 
    mesh.Visibility=1
    reset_view(aView)
    Render()

    # Add path separator to the end of picture path if necessery
    if not picturedir.endswith(os.sep):
            picturedir += os.sep
    entity_name = EntityType.get_name(entity_type)
    # Construct image file name
    pic_name = "{folder}{mesh}_{type}.{ext}".format(folder=picturedir,
                                                                        mesh=mesh_name,
                                                                        type=entity_name,
                                                                        ext=pictureext)
    
    process_prs_for_test(mesh, aView, pic_name)



