import os
import salome

# Auxiliary classes
class SalomeSession(object):
    def __init__(self):
        import runSalome
        import sys
        sys.argv += ["--show-desktop=1"]
	sys.argv += ["--splash=0"]
        sys.argv += ["--modules=PARAVIS"]
        clt, d = runSalome.main()
        port = d['port']
        self.port = port
        return
    def __del__(self):
        from os import system
        system('killSalomeWithPort.py %s'%(self.port))
        return
    pass

# Auxiliary functions
def TestValues(Value, Etvalue, CHECK_ERROR=0):
    error=0
    length = len (Value)
    Etlength = len (Etvalue)
    if (length != Etlength):
        print "ERROR!!! There is different number of created ",length, " and etalon ", Etlength, " values!!!"
	error = error+1
    else:
        for i in range(Etlength):
	    if abs(Etvalue[i]) > 1:
	        MAX = abs(0.001*Etvalue[i])
		if abs(Etvalue[i] -  Value[i]) > MAX: 
		    print "ERROR!!! Got value ", Value[i], " is not equal to etalon value!!!", Etvalue[i]
		    error=error+1
	    else:
	        MAX = 0.001
		if abs(Etvalue[i] -  Value[i])> MAX:
		    print "ERROR!!! Got value", Value[i], " is not equal to etalon value!!!", Etvalue[i]
		    error=error+1
    if CHECK_ERROR and error > 0:
       raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
    return error

# Auxiliary variables

# Set directories
datadir = os.getenv("DATA_DIR")
if datadir is not None:
    datadir=datadir+ "/MedFiles/"

output = os.getenv("CASE_LOG_FILE")

pic_dir_suf = os.getenv("PIC_DIR_SUFFICS")

if pic_dir_suf == None:
    pic_dir_suf = "_dat"

if output is not None:
    picturedir = output + pic_dir_suf + "/"
else:
    picturedir = "/tmp/pic/"

if not os.path.exists(picturedir) :
    os.mkdir(picturedir)
#MZN:
#else:   #clear the old pictures in the dir
#   	dir = os.listdir(picturedir)
#	for item in dir :
#		fitem = picturedir + "/" + item
#		if os.path.isfile(fitem):
#			os.remove(fitem)

pictureext = os.getenv("PIC_EXT");
if pictureext == None:
    pictureext="bmp"

# Run Salome
salome_session = SalomeSession()
salome.salome_init()

# Create new study
#import visu_gui #@MZN: to be reimplemented

# Create new study
print "Creating new study...",
aStudy = salome.myStudyManager.NewStudy("Study1")
if aStudy is None: 
    raise RuntimeError, "Error"
else: 
    print "OK"

salome.myStudy = aStudy
