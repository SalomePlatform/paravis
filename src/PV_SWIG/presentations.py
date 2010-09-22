"""
This module is intended to provide Python API for building presentations
typical for Post-Pro module (Scalar Map, Deformed Shape, Vectors, etc.)
"""

import re

try:
    pvsimple
except:
    from pvsimple import *

# Constants
EPS = 0.001
DEBUG_ON = True

# Enumerations
class PrsTypeEnum:
    """
    Presentation types.
    """
    SCALARMAP = 1
    ISOSURFACES = 2
    CUTPLANES = 3
    CUTLINES = 4
    DEFORMEDSHAPE = 5
    DEFORMEDSHAPESCALARMAP = 6
    VECTORS = 7

class Orientation:
    """
    Orientation types.
    """
    XY = 1
    YZ = 2
    ZX = 3

def ProcessPrsForTest(thePrs, theView, thePictureName):
    """Auxiliary function. Used to show presentation and record image."""
    # Show 
    thePrs.Visibility = 1 
    
    # Update view 
    Render(view=theView) 
    ResetView(theView) 
    ResetCamera(view=theView) 
           
    # Save picture 
    aPictureName = re.sub("\s+","_", thePictureName) 
    WriteImage(aPictureName, view=theView, Magnification=1) 
    
    # Hide all
    HideAll(theView, True)
    Render(view=theView)

def ResetView(theView=None):
    """Auxiliary function. Resets the settings of the camera.
    If an argument is not provided, the active view is used."""
    if not theView:
        theView = GetRenderView()

    theView.CameraFocalPoint = [0.0, 0.0, 0.0]
    theView.CameraViewUp = [0.0, 0.0, 1.0]
    theView.CameraPosition = [738.946, -738.946, 738.946]

def HideAll(theView, toRemove = False):
    """Auxiliary function. Hide all representations for the given view."""
    if (theView is None):
        return
    aRepLst = theView.Representations
    for rep in aRepLst:
        if (hasattr(rep, 'Visibility') and rep.Visibility != 0):
            rep.Visibility = 0
        if (toRemove):
            theView.Representations.remove(rep)

def DisplayOnly(theView, thePrs):
    HideAll(theView)
    if (hasattr(thePrs, 'Visibility') and thePrs.Visibility != 1):
        thePrs.Visibility = 1

def HasValue(theList, theValue):
    """Auxiliary function."""
    try :
        return theList.index(theValue) + 1
    except StandardError, e :
        return 0
    
def CheckVectorMode(theVectorMode, theNbComponents):
    """Auxiliary function. Check vector mode."""
    vectorMode = theVectorMode
    if (theNbComponents == 1 and (theVectorMode=='Magnitude' or
                                  theVectorMode=='Y' or
                                  theVectorMode=='Z')):
        vectorMode = 'X'
    elif (theNbComponents == 2 and  theVectorMode=='Z'):
        vectorMode = 'X'

    if (vectorMode != theVectorMode and theVectorMode != 'Magnitude'):
        print "Set vector mode to \"%s\"" % vectorMode

    return vectorMode

def GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode):
    """Auxiliary function. Get data range for the field."""
    entityDataInfo = None
    if (theEntityType == 'CELL_DATA'):
        entityDataInfo = theProxy.GetCellDataInformation()
    else:
        entityDataInfo = theProxy.GetPointDataInformation()

    dataRange = []
    
    if (theVectorMode=='Magnitude'):
        dataRange = entityDataInfo[theFieldName].GetComponentRange(-1)
    elif (theVectorMode=='X'):
        dataRange = entityDataInfo[theFieldName].GetComponentRange(0)
    elif (theVectorMode=='Y'):
        dataRange = entityDataInfo[theFieldName].GetComponentRange(1)
    elif (theVectorMode=='Z'):
        dataRange = entityDataInfo[theFieldName].GetComponentRange(2)
    else:
        raise RuntimeError, "Vector mode is incorrect: %s " % theVectorMode

    return dataRange
        
def GetXRange(theProxy):
    """Auxiliary function. Get X range."""
    dataInfo = theProxy.GetDataInformation()
    boundsInfo = dataInfo.GetBounds()
    return boundsInfo[0:2]

def GetYRange(theProxy):
    """Auxiliary function. Get Y range."""
    dataInfo = theProxy.GetDataInformation()
    boundsInfo = dataInfo.GetBounds()
    return boundsInfo[2:4]

def GetZRange(theProxy):
    """Auxiliary function. Get Z range."""
    dataInfo = theProxy.GetDataInformation()
    boundsInfo = dataInfo.GetBounds()
    return boundsInfo[4:6]

def GetPositions(valRange, theNbPlanes, theDisplacement):
    """Compute plane positions."""
    positions = []
    if (theNbPlanes > 1):
        step = (valRange[1] - valRange[0])/(theNbPlanes-1)
        startPos = valRange[0] + step*(theDisplacement - 0.5)
        for i in range(theNbPlanes):
            pos = startPos + i*step
            positions.append(pos)
    elif (theNbPlanes == 1):
        pos = valRange[0] + (valRange[1] - valRange[0])*theDisplacement
        positions.append(pos)
        
    return positions
    
def GetNbComponents(theProxy, theEntityType, theFieldName):
    """Auxiliary function. Return number of components for the field."""
    entityDataInfo = None
    if (theEntityType == 'CELL_DATA'):
        entityDataInfo = theProxy.GetCellDataInformation()
    else:
        entityDataInfo = theProxy.GetPointDataInformation()
    nbComponents = entityDataInfo[theFieldName].GetNumberOfComponents()
    return nbComponents

def GetStdScalarBar(theTitle, theLookupTable):
    """Auxiliary function. Create scalar bar."""
    scalarBar = CreateScalarBar(Enabled=1, LabelFontSize=10, TitleFontSize=8, NumberOfLabels=5)
    scalarBar.Title = theTitle
    scalarBar.LookupTable = theLookupTable
    return scalarBar

def GetStdLookupTable(theFieldName, theNbComp, theVectorMode='Magnitude'):
    """Auxiliary function. Get lookup table for the given field."""
    lookupTable = GetLookupTableForArray(theFieldName, theNbComp)

    if (theVectorMode=='Magnitude'):
        lookupTable.VectorMode=theVectorMode
    elif (theVectorMode=='X'):
        lookupTable.VectorMode = 'Component'
        lookupTable.VectorComponent = 0
    elif (theVectorMode=='Y'):
        lookupTable.VectorMode = 'Component'
        lookupTable.VectorComponent = 1
    elif (theVectorMode=='Z'):
        lookupTable.VectorMode = 'Component'
        lookupTable.VectorComponent = 2
    else:
        raise RuntimeError, "Vector mode is incorrect: %s " % theVectorMode
        
    lookupTable.Discretize = 0
    lookupTable.ColorSpace = 'HSV'
    lookupTable.LockScalarRange = 0
    
    return lookupTable

def GetTime(theProxy, theTimeStampNumber):
    """Auxiliary function. Get time value by timestamp number."""
    # Check timestamp number
    timeStamps = theProxy.TimestepValues.GetData()
    nbTimeStamps = len(timeStamps)
    if (theTimeStampNumber < 1 or theTimeStampNumber > nbTimeStamps):
        raise RuntimeError, "Timestamp number is out of range: %s " % theTimeStampNumber

    # Return time value
    return timeStamps[theTimeStampNumber-1]
    
def ScalarMapOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber, theVectorMode='Magnitude'):
    """Creates scalar map on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)

    # Show pipeline object
    scalarMap = GetRepresentation(theProxy)
   
    # Get lookup table
    nbComponents = GetNbComponents(theProxy, theEntityType, theFieldName)
    theVectorMode = CheckVectorMode(theVectorMode, nbComponents)
    lookupTable = GetStdLookupTable(theFieldName, nbComponents, theVectorMode)

    # Set field range if necessary
    dataRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookupTable.LockScalarRange = 1
    lookupTable.RGBPoints = [dataRange[0], 0, 0, 1, dataRange[1], 1, 0, 0]

    # Set properties
    scalarMap.ColorAttributeType = theEntityType
    scalarMap.ColorArrayName = theFieldName
    scalarMap.LookupTable = lookupTable

    # Set scalar bar name and lookup table
    barTitle = theFieldName + ", " + str(timeValue)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue

    return scalarMap

def CutPlanesOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                     theNbPlanes=10, theOrientation = Orientation.YZ, theDisplacement=0.5,
                     theVectorMode='Magnitude'):
    """Creates cut planes on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)

    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0
    
    # Create slice filter
    sliceFilter=Slice(theProxy)
    sliceFilter.SliceType="Plane"

    # Set cut planes normal and get range
    normal = [0.0, 0.0, 0.0]
    valRange = []
    if (theOrientation == Orientation.XY):
        normal[2] = 1.0
        valRange = GetZRange(theProxy)
    elif (theOrientation == Orientation.ZX):
        normal[1] = 1.0
        valRange = GetYRange(theProxy)
    elif (theOrientation == Orientation.YZ):
        normal[0] = 1.0
        valRange = GetXRange(theProxy)
        
    sliceFilter.SliceType.Normal = normal

    # Set cut planes positions
    sliceFilter.SliceOffsetValues = GetPositions(valRange, theNbPlanes, theDisplacement)
    cutPlanes = GetRepresentation(sliceFilter)
   
    # Get lookup table
    nbComponents = GetNbComponents(theProxy, theEntityType, theFieldName)
    theVectorMode = CheckVectorMode(theVectorMode, nbComponents)
    lookupTable = GetStdLookupTable(theFieldName, nbComponents, theVectorMode)

    # Set field range if necessary
    dataRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookupTable.LockScalarRange = 1
    lookupTable.RGBPoints = [dataRange[0], 0, 0, 1, dataRange[1], 1, 0, 0]

    # Set properties
    cutPlanes.ColorAttributeType = theEntityType
    cutPlanes.ColorArrayName = theFieldName
    cutPlanes.LookupTable = lookupTable

    # Set scalar bar name and lookup table
    barTitle = theFieldName + ", " + str(timeValue)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue
    
    return cutPlanes

def CutLinesOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                    theNbLines=10,
                    theOrientation1 = Orientation.XY, theOrientation2 = Orientation.YZ,
                    theDisplacement1=0.5, theDisplacement2=0.5,
                    theVectorMode='Magnitude'):
    """Creates cut lines on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)
    
    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0
    
    # Create base plane
    basePlane=Slice(theProxy)
    basePlane.SliceType="Plane"

    # Set base plane normal and get position
    normalBase = [0.0, 0.0, 0.0]
    valRangeBase = []
    if (theOrientation1 == Orientation.XY):
        normalBase[2] = 1.0
        valRangeBase = GetZRange(theProxy)
    elif (theOrientation1 == Orientation.ZX):
        normalBase[1] = 1.0
        valRangeBase = GetYRange(theProxy)
    elif (theOrientation1 == Orientation.YZ):
        if (theOrientation2 == theOrientation1):
            theOrientation2=Orientation.ZX
        normalBase[0] = 1.0
        valRangeBase = GetXRange(theProxy)
        
    basePlane.SliceType.Normal = normalBase

    # Set base plane position
    basePlane.SliceOffsetValues = GetPositions(valRangeBase, 1, theDisplacement1)
    
    basePlaneRep = GetRepresentation(basePlane)
    basePlaneRep.Visibility = 0
    
    # Create cutting planes
    cutPlanes=Slice()
    cutPlanes.SliceType="Plane"

    # Set cutting planes normal and get positions
    normalCut = [0.0, 0.0, 0.0]
    valRangeCut = []
    if (theOrientation2 == Orientation.XY):
        normalCut[2] = 1.0
        valRangeCut = GetZRange(theProxy)
    elif (theOrientation2 == Orientation.ZX):
        normalCut[1] = 1.0
        valRangeCut = GetYRange(basePlane)
    elif (theOrientation2 == Orientation.YZ):
        normalCut[0] = 1.0
        valRangeCut = GetXRange(basePlane)
        
    cutPlanes.SliceType.Normal = normalCut

    # Set cutting planes position
    cutPlanes.SliceOffsetValues = GetPositions(valRangeCut, theNbLines, theDisplacement2)
    cutLines = GetRepresentation(cutPlanes)
   
    # Get lookup table
    nbComponents = GetNbComponents(theProxy, theEntityType, theFieldName)
    theVectorMode = CheckVectorMode(theVectorMode, nbComponents)
    lookupTable = GetStdLookupTable(theFieldName, nbComponents, theVectorMode)

    # Set field range if necessary
    dataRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookupTable.LockScalarRange = 1
    lookupTable.RGBPoints = [dataRange[0], 0, 0, 1, dataRange[1], 1, 0, 0]
    
    # Set properties
    cutLines.ColorAttributeType = theEntityType
    cutLines.ColorArrayName = theFieldName
    cutLines.LookupTable = lookupTable

    # Set scalar bar name and lookup table
    barTitle = theFieldName + ", " + str(timeValue)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    # Show scalar bar
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue

    return cutLines

def VectorsOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                   theIsColored=False, theVectorMode='Magnitude'):
    """Creates vectors on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)

    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0
    
    # Cell centers
    cellCenters = CellCenters()

    # Vectors
    glyphFilter = Glyph(GlyphType="Arrow")
    glyphFilter.Vectors = theFieldName
    glyphFilter.ScaleMode = 'vector'
    
    vectors = GetRepresentation(glyphFilter)
   
    # Get lookup table
    nbComponents = GetNbComponents(theProxy, theEntityType, theFieldName)
    theVectorMode = CheckVectorMode(theVectorMode, nbComponents)
    lookupTable = GetStdLookupTable(theFieldName, nbComponents, theVectorMode)

    # Set field range if necessary
    dataRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookupTable.LockScalarRange = 1
    lookupTable.RGBPoints = [dataRange[0], 0, 0, 1, dataRange[1], 1, 0, 0]

    # Set properties
    if (theIsColored):
        vectors.ColorArrayName = 'GlyphVector'
    vectors.LookupTable = lookupTable

    # Set scalar bar name and lookup table
    barTitle = theFieldName + ", " + str(timeValue)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue
    
    return vectors

def DeformedShapeOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber, theScaleFactor=-1,
                         theIsColored=False, theVectorMode='Magnitude'):
    """Creates and return Defromed shape on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)
    
    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    bergeBlocks = MergeBlocks()
    
    # Cell centers
    cellDataToPointData = CellDatatoPointData()

    # Warp by vector
    warpByVector = WarpByVector()
    warpByVector.Vectors = [theFieldName]
    if (theScaleFactor > 0):
        warpByVector.ScaleFactor = theScaleFactor
    
    defshape = GetRepresentation(warpByVector)
   
    # Get lookup table
    nbComponents = GetNbComponents(theProxy, theEntityType, theFieldName)
    theVectorMode = CheckVectorMode(theVectorMode, nbComponents)
    lookupTable = GetStdLookupTable(theFieldName, nbComponents, theVectorMode)

    # Set field range if necessary
    dataRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookupTable.LockScalarRange = 1
    lookupTable.RGBPoints = [dataRange[0], 0, 0, 1, dataRange[1], 1, 0, 0]

    # Set properties
    if (theIsColored):
        defshape.ColorAttributeType = theEntityType
        defshape.ColorArrayName = theFieldName
    else:
        defshape.ColorArrayName = ''
    defshape.LookupTable = lookupTable

    # Set scalar bar name and lookup table
    barTitle = theFieldName + ", " + str(timeValue)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue

    # Set wireframe represenatation mode
    defshape.Representation = 'Wireframe'
    
    return defshape

def DeformedShapeAndScalarMapOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber, theScaleFactor=-1,
                                     theScalarEntityType=None, theScalarFieldName=None,
                                     theVectorMode='Magnitude'):
    """Creates and Defromed shape And Scalar Map on the given field."""
    # Set scalar field by default
    if ((theScalarEntityType is None) or (theScalarFieldName is None)):
        theScalarEntityType = theEntityType
        theScalarFieldName = theFieldName
        
     # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)
    
    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    bergeBlocks = MergeBlocks()
    
    # Cell centers
    cellDataToPointData = CellDatatoPointData()

    # Warp by vector
    warpByVector = WarpByVector()
    warpByVector.Vectors = [theFieldName]
    if (theScaleFactor > 0):
        warpByVector.ScaleFactor = theScaleFactor
    
    defshapeandmap = GetRepresentation(warpByVector)
   
    # Get lookup table
    nbComponents = GetNbComponents(theProxy, theScalarEntityType, theScalarFieldName)
    theVectorMode = CheckVectorMode(theVectorMode, nbComponents)
    lookupTable = GetStdLookupTable(theScalarFieldName, nbComponents, theVectorMode)

    # Set field range if necessary
    dataRange = GetDataRange(theProxy, theScalarEntityType, theScalarFieldName, theVectorMode)
    lookupTable.LockScalarRange = 1
    lookupTable.RGBPoints = [dataRange[0], 0, 0, 1, dataRange[1], 1, 0, 0]
    
    # Set properties
    defshapeandmap.ColorAttributeType = theScalarEntityType
    defshapeandmap.ColorArrayName = theScalarFieldName
       
    defshapeandmap.LookupTable = lookupTable

    # Set scalar bar name and lookup table
    barTitle = theScalarFieldName + ", " + str(timeValue)
    nbComponents = GetNbComponents(theProxy, theScalarEntityType, theScalarFieldName)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue
       
    return defshapeandmap

def CreatePrsForFile(theParavis, theFileName, thePrsTypeList, thePictureDir, thePictureExt, theIsAutoDelete = 0):
    """Build presentations of the given types for all fields of the given file."""
    print "Import %s..." % theFileName.split('/')[-1],
    theParavis.ImportFile(theFileName)
    aProxy = GetActiveSource()
    if aProxy is None:
        raise RuntimeError, "Error: can't import file."
    else: print "OK"

    aView = GetRenderView()

    CreatePrsForProxy(aProxy, aView, thePrsTypeList, thePictureDir, thePictureExt, theIsAutoDelete)

def CreatePrsForProxy(theProxy, theView, thePrsTypeList, thePictureDir, thePictureExt, theIsAutoDelete):
    """Build presentations of the given types for all fields of the given proxy."""
    aFieldNames = theProxy.PointArrays.GetData()
    aNbOnNodes = len(aFieldNames)
    aFieldNames.extend(theProxy.CellArrays.GetData())
    aTimeStamps = theProxy.TimestepValues.GetData()
    
    for i in xrange(len(aFieldNames)):
        aFieldName = aFieldNames[i]
        aFieldEntity = 'POINT_DATA'
        if (i >= aNbOnNodes):
            aFieldEntity = 'CELL_DATA'
        
        if HasValue(thePrsTypeList, PrsTypeEnum.SCALARMAP):
            # Create Scalar Map for all timestamps
            for timeStampNb in xrange(1, len(aTimeStamps)+1):
                timeValue = aTimeStamps[timeStampNb-1]
                print "          Creating Scalar Map on %s, time = %s... " % (aFieldName, str(timeValue)),
                aPrs = ScalarMapOnField(theProxy, aFieldEntity, aFieldName, timeStampNb)
                if aPrs is None :
                    print "Error: can't create Scalar Map." 
                else:
                    print "OK"

                # Construct image file name
                aPictureName = thePictureDir + aFieldName + "_" + str(timeValue) + "_SCALARMAP." + thePictureExt 

                # Show and record the presentation
                ProcessPrsForTest(aPrs, theView, aPictureName)
                
        if HasValue(thePrsTypeList, PrsTypeEnum.CUTPLANES):
            # Create Cut Planes for all timestamps
            for timeStampNb in xrange(1, len(aTimeStamps)+1):
                timeValue = aTimeStamps[timeStampNb-1]
                print "          Creating Cut Planes on %s, time = %s... " % (aFieldName, str(timeValue)),
                aPrs = CutPlanesOnField(theProxy, aFieldEntity, aFieldName, timeStampNb)
                if aPrs is None :
                    print "Error: can't create Cut Planes." 
                else:
                    print "OK" 

                # Construct image file name
                aPictureName = thePictureDir + aFieldName + "_" + str(timeValue) + "_CUTPLANES." + thePictureExt

                # Show and record the presentation
                ProcessPrsForTest(aPrs, theView, aPictureName)

        if HasValue(thePrsTypeList, PrsTypeEnum.DEFORMEDSHAPE):
            # Create  Deformed Shape for all timestamps
            for timeStampNb in xrange(1, len(aTimeStamps)+1):
                timeValue = aTimeStamps[timeStampNb-1]
                print "          Creating Deformed Shape on %s, time = %s... " % (aFieldName, str(timeValue)),
                aPrs = DeformedShapeOnField(theProxy, aFieldEntity, aFieldName, timeStampNb)
                if aPrs is None :
                    print "Error: can't create Deformed Shape." 
                else:
                    print "OK" 

                # Construct image file name
                aPictureName = thePictureDir + aFieldName + "_" + str(timeValue) + "_DEFORMEDSHAPE." + thePictureExt

                # Show and record the presentation
                ProcessPrsForTest(aPrs, theView, aPictureName)

    # Delete the proxy if necessary
    if (theIsAutoDelete):
        Delete(proxy=theProxy)
