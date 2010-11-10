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
FLT_MIN = 1E-37
VTK_LARGE_FLOAT = 1E+38

DEBUG_ON = True

# Enumerations
class PrsTypeEnum:
    """
    Presentation types.
    """
    MESH = 0
    SCALARMAP = 1
    ISOSURFACES = 2
    CUTPLANES = 3
    CUTLINES = 4
    DEFORMEDSHAPE = 5
    DEFORMEDSHAPESCALARMAP = 6
    VECTORS = 7
    PLOT3D = 8
    STREAMLINES = 9

class Orientation:
    """
    Orientation types.
    """
    AUTO = 0
    XY = 1
    YZ = 2
    ZX = 3

class _aux_counter:
    "Internal class."
    first_render = True

def ProcessPrsForTest(thePrs, theView, thePictureName):
    """Auxiliary function. Used to show presentation and record image."""
    # Show
    thePrs.Visibility = 1 
    
    # Reset view 
    ResetView(theView) 
           
    # Save picture 
    aPictureName = re.sub("\s+","_", thePictureName) 
    WriteImage(aPictureName, view=theView, Magnification=1) 
    
    # Hide all
    HideAll(theView, True)

def ResetView(theView=None):
    """Auxiliary function. Resets the view.
    If an argument is not provided, the active view is used."""
    if not theView:
        theView = GetRenderView()

    if _aux_counter.first_render:
        theView.CameraFocalPoint = [0.0, 0.0, 0.0]
        theView.CameraViewUp = [0.0, 0.0, 1.0]
        theView.CameraPosition = [738.946, -738.946, 738.946]
        _aux_counter.first_render = False

    theView.ResetCamera()
    Render(view=theView)

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

def GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode='Magnitude'):
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

def GetBounds(theProxy):
    """Auxiliary function. Get bounds of the proxy."""
    dataInfo = theProxy.GetDataInformation()
    boundsInfo = dataInfo.GetBounds()

    return boundsInfo

def GetXRange(theProxy):
    """Auxiliary function. Get X range."""
    boundsInfo = GetBounds(theProxy)
    return boundsInfo[0:2]

def GetYRange(theProxy):
    """Auxiliary function. Get Y range."""
    boundsInfo = GetBounds(theProxy)
    return boundsInfo[2:4]

def GetZRange(theProxy):
    """Auxiliary function. Get Z range."""
    boundsInfo = GetBounds(theProxy)
    return boundsInfo[4:6]

def IsPlanarInput(theProxy):
    """Auxiliary function. Check if the given input is planar."""
    boundsInfo = GetBounds(theProxy)

    if ( abs(boundsInfo[0] - boundsInfo[1]) <= FLT_MIN or
         abs(boundsInfo[2] - boundsInfo[3]) <= FLT_MIN or
         abs(boundsInfo[4] - boundsInfo[5]) <= FLT_MIN ):
        return True

    return False

def IsDataOnCells(theProxy, theFieldName):
    """Auxiliary function. Check if the field data is on cells."""
    cellDataInfo = theProxy.GetCellDataInformation()
    return (theFieldName in cellDataInfo.keys())

def GetOrientation(theProxy):
    """Auxiliary function. Get the optimum cutting plane orientation."""
    anOrientation = Orientation.XY

    aBounds = GetBounds(theProxy)
    aDelta = [aBounds[1] - aBounds[0],
              aBounds[3] - aBounds[2],
              aBounds[5] - aBounds[4] ]

    if (aDelta[0] >= aDelta[1] and aDelta[0] >= aDelta[2]):
        if (aDelta[1] >= aDelta[2]):
            anOrientation = Orientation.XY
        else:
            anOrientation = Orientation.ZX
    elif (aDelta[1] >= aDelta[0] and aDelta[1] >= aDelta[2]):
        if (aDelta[0] >= aDelta[2]):
            anOrientation = Orientation.XY
        else:
            anOrientation = Orientation.YZ
    elif (aDelta[2] >= aDelta[0] and aDelta[2] >= aDelta[1]):
        if (aDelta[0] >= aDelta[1]):
            anOrientation = Orientation.ZX
        else:
            anOrientation = Orientation.YZ

    return anOrientation

def GetNormalByOrientation(theOrientation):
    """Auxiliary function. Get normal for the plane by its orientation."""
    normal = [0.0, 0.0, 0.0]
    if (theOrientation == Orientation.XY):
        normal[2] = 1.0
    elif (theOrientation == Orientation.ZX):
        normal[1] = 1.0
    elif (theOrientation == Orientation.YZ):
        normal[0] = 1.0
        
    return normal

def GetRangeForOrientation(theProxy, theOrientation):
    """Auxiliary function. Get source range for cutting plane orientation."""
    valRange = []
    if (theOrientation == Orientation.XY):
        valRange = GetZRange(theProxy)
    elif (theOrientation == Orientation.ZX):
        valRange = GetYRange(theProxy)
    elif (theOrientation == Orientation.YZ):
        valRange = GetXRange(theProxy)

    return valRange

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

def GetScaleFactor(theProxy):
    """Auxiliary function. Get scale factor."""
    aDataInfo = theProxy.GetDataInformation()

    aNbCells = aDataInfo.GetNumberOfCells()
    aNbPoints = aDataInfo.GetNumberOfPoints()
    aNbElements = aNbCells
    if (aNbElements < 1):
        aNbElements = aNbPoints

    aBounds = GetBounds(theProxy)

    aVolume = 1
    aVol = aDim = 0
    
    for i in xrange(0, 6, 2):
        aVol = abs(aBounds[i+1] - aBounds[i])
        if (aVol > 0):
            aDim += 1
            aVolume *= aVol

    if (aNbElements == 0 or aDim < 1):
        return 0

    aVolume /= aNbElements
    return pow(aVolume, 1.0/aDim)
            
def GetDefaultScale(theProxy, theEntityType, theFieldName):
    """Auxiliary function. Get default scale factor."""
    aDataRange = GetDataRange(theProxy, theEntityType, theFieldName)
    EPS = 1.0 / VTK_LARGE_FLOAT
    if abs(aDataRange[1] > EPS):
        aScaleFactor = GetScaleFactor(theProxy)
        return aScaleFactor / aDataRange[1];
    
    return 0
    

def IfPossible(theProxy, theEntityType, theFieldName, thePrsType):
    """Auxiliary function. Check if the presentation is possible on the given field."""
    result = False
    if (thePrsType == PrsTypeEnum.DEFORMEDSHAPE or
        thePrsType == PrsTypeEnum.DEFORMEDSHAPESCALARMAP or
        thePrsType == PrsTypeEnum.VECTORS):
        nbComp = GetNbComponents(theProxy, theEntityType, theFieldName)
        result = (nbComp > 1)
        
    return result

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
    cutPlanes=Slice(basePlane)
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
    cellCenters = CellCenters(theProxy)

    # Vectors
    glyphFilter = Glyph(cellCenters)
    glyphFilter.GlyphType="Arrow"
    glyphFilter.Vectors = theFieldName
    glyphFilter.ScaleMode = 'vector'

    glyphFilter.GlyphType.TipResolution = 1
    glyphFilter.GlyphType.TipRadius = 0.1
    glyphFilter.GlyphType.TipLength = 0.19
    glyphFilter.GlyphType.ShaftResolution = 0
    glyphFilter.GlyphType.ShaftRadius = 0.0
    
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

    # Set wireframe represenatation mode
    vectors.Representation = 'Wireframe'

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
    """Creates Defromed shape on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)
    
    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    mergeBlocks = MergeBlocks(theProxy)
    
    # Cell data to point data
    if (IsDataOnCells(theProxy, theFieldName)):
        cellDataToPointData = CellDatatoPointData()
        cellDataToPointData.PassCellData = 1
        
    # Warp by vector
    warpByVector = WarpByVector()
    warpByVector.Vectors = [theFieldName]
    if (theScaleFactor > 0):
        warpByVector.ScaleFactor = theScaleFactor
    else:
        aDefScale = GetDefaultScale(theProxy, theEntityType, theFieldName)
        warpByVector.ScaleFactor = aDefScale
        
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
    """Creates Defromed shape And Scalar Map on the given field."""
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
    mergeBlocks = MergeBlocks(theProxy)

    # Cell data to point data
    if (IsDataOnCells(theProxy, theFieldName)):
        cellDataToPointData = CellDatatoPointData()
        cellDataToPointData.PassCellData = 1
        
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
    defshapeandmap.ColorArrayName = theScalarFieldName
    defshapeandmap.LookupTable = lookupTable
    defshapeandmap.ColorAttributeType = theScalarEntityType

    #Render()
    
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

def Plot3DOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                  theOrientation = Orientation.AUTO, thePosition=0.5, theIsRelative=True,
                  theScaleFactor=-1, theIsContourPrs=False, theNbOfContours=32,
                  theVectorMode='Magnitude'):
    """Creates plot 3d on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)

    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    mergeBlocks = MergeBlocks(theProxy)
    mergeBlocks.UpdatePipeline()
    #Show()
    
    aPolyData = None

    # Define orientation if necessary (auto mode)
    anOrientation = theOrientation
    if (anOrientation == Orientation.AUTO):
        anOrientation = GetOrientation(theProxy)

    # Get cutting plane normal
    normal = GetNormalByOrientation(anOrientation)
   
    # Cutting plane
    if (not IsPlanarInput(theProxy)):
        # Create slice filter
        sliceFilter=Slice(mergeBlocks)
        sliceFilter.SliceType="Plane"

        # Set cutting plane normal
        sliceFilter.SliceType.Normal = normal

        # Set cutting plane position
        valRange = GetRangeForOrientation(theProxy, anOrientation)
        
        if (theIsRelative):
            sliceFilter.SliceOffsetValues = GetPositions(valRange, 1, thePosition)
        else:
            sliceFilter.SliceOffsetValues = thePosition

        sliceFilter.UpdatePipeline()
        aPolyData = sliceFilter

    # Geometry filter
    if (not aPolyData or aPolyData.GetDataInformation().GetNumberOfCells() == 0):
        geometryFilter = GeometryFilter(mergeBlocks)
        aPolyData = geometryFilter
    
    warpByScalar = None
    plot3d = None

    if (IsDataOnCells(aPolyData, theFieldName)):
        # Cell data to point data
        cellDataToPointData = CellDatatoPointData(aPolyData)
        cellDataToPointData.PassCellData = 1
        # Warp by scalar
        warpByScalar = WarpByScalar(cellDataToPointData)
    else:
        # Warp by scalar
        warpByScalar = WarpByScalar(aPolyData)

    # Warp by scalar settings
    warpByScalar.Scalars = ['POINTS', theFieldName]
    warpByScalar.Normal = normal
    if (theScaleFactor > 0):
        warpByScalar.ScaleFactor = theScaleFactor
    warpByScalar.UpdatePipeline()

    if (theIsContourPrs):
        # Contours
        contour = Contour(warpByScalar)
        contour.PointMergeMethod = "Uniform Binning"
        contour.ContourBy = ['POINTS', theFieldName]
        scalarRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
        contour.Isosurfaces = GetPositions(scalarRange, theNbOfContours, 0)
        contour.UpdatePipeline()
        plot3d = GetRepresentation(contour)
    else:
        plot3d = GetRepresentation(warpByScalar)
   
    # Get lookup table
    nbComponents = GetNbComponents(theProxy, theEntityType, theFieldName)
    theVectorMode = CheckVectorMode(theVectorMode, nbComponents)
    lookupTable = GetStdLookupTable(theFieldName, nbComponents, theVectorMode)

    # Set field range if necessary
    dataRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookupTable.LockScalarRange = 1
    lookupTable.RGBPoints = [dataRange[0], 0, 0, 1, dataRange[1], 1, 0, 0]

    # Set properties
    plot3d.ColorAttributeType = theEntityType
    plot3d.ColorArrayName = theFieldName
    plot3d.LookupTable = lookupTable
  
    # Set scalar bar name and lookup table
    barTitle = theFieldName + ", " + str(timeValue)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue
    
    return plot3d

def IsoSurfacesOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                       theRange = None, theNbOfSurfaces=10, theIsColored = True,
                       theColor = None, theVectorMode='Magnitude'):
    """Creates iso surfaces on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)
    
    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0

    contourFilter = None

    if (IsDataOnCells(theProxy, theFieldName)):
        # Cell data to point data
        cellDataToPointData = CellDatatoPointData(theProxy)
        cellDataToPointData.PassCellData = 1
        # Contour
        contourFilter = Contour(cellDataToPointData)
    else:
        contourFilter = Contour(theProxy)

    # Contour filter settings
    contourFilter.PointMergeMethod = "Uniform Binning"
    contourFilter.ContourBy = ['POINTS', theFieldName]
    scalarRange = theRange
    if (scalarRange is None):
        scalarRange = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    contourFilter.Isosurfaces = GetPositions(scalarRange, theNbOfSurfaces, 0)
    contourFilter.UpdatePipeline()

    isosurfaces = GetRepresentation(contourFilter)

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
        isosurfaces.ColorAttributeType = theEntityType
        isosurfaces.ColorArrayName = theFieldName
    else:
        isosurfaces.ColorArrayName = ''
        if (theColor is not None):
            isosurfaces.DiffuseColor = theColor
    isosurfaces.LookupTable = lookupTable
  
    # Set scalar bar name and lookup table
    barTitle = theFieldName + ", " + str(timeValue)
    if (nbComponents > 1):
        barTitle += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(barTitle, lookupTable)
    GetRenderView().Representations.append(scalarBar)

    # Set timestamp
    GetRenderView().ViewTime = timeValue

    return isosurfaces

def GaussPointsOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                       theVectorMode='Magnitude'):
    """Creates gauss points on the given field."""
    # Get time value
    timeValue = GetTime(theProxy, theTimeStampNumber)
    
    # Hide initial object
    rep = GetRepresentation(theProxy)
    rep.Visibility = 0

    gausspnt = None

    return gausspnt

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

        if HasValue(thePrsTypeList, PrsTypeEnum.CUTLINES):
            # Create Cut Lines for all timestamps
            for timeStampNb in xrange(1, len(aTimeStamps)+1):
                timeValue = aTimeStamps[timeStampNb-1]
                print "          Creating Cut Lines on %s, time = %s... " % (aFieldName, str(timeValue)),
                aPrs = CutLinesOnField(theProxy, aFieldEntity, aFieldName, timeStampNb)
                if aPrs is None :
                    print "Error: can't create Cut Lines." 
                else:
                    print "OK" 

                # Construct image file name
                aPictureName = thePictureDir + aFieldName + "_" + str(timeValue) + "_CUTLINES." + thePictureExt

                # Show and record the presentation
                ProcessPrsForTest(aPrs, theView, aPictureName)

        if HasValue(thePrsTypeList, PrsTypeEnum.DEFORMEDSHAPE):
            # Create  Deformed Shape for all timestamps if possible
            aIsPossible = IfPossible(theProxy, aFieldEntity, aFieldName, PrsTypeEnum.DEFORMEDSHAPE)
            if (aIsPossible):
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

        if HasValue(thePrsTypeList, PrsTypeEnum.DEFORMEDSHAPESCALARMAP):
            # Create Scalar Map on Deformed Shape for all timestamps if possible
            aIsPossible = IfPossible(theProxy, aFieldEntity, aFieldName, PrsTypeEnum.DEFORMEDSHAPESCALARMAP)
            if (aIsPossible):
                for timeStampNb in xrange(1, len(aTimeStamps)+1):
                    timeValue = aTimeStamps[timeStampNb-1]
                    print "          Creating Scalar Map on Deformed Shape on %s, time = %s... " % (aFieldName, str(timeValue)),
                    aPrs = DeformedShapeAndScalarMapOnField(theProxy, aFieldEntity, aFieldName, timeStampNb, 0.25)
                    if aPrs is None :
                        print "Error: can't create Scalar Map on Deformed Shape." 
                    else:
                        print "OK" 

                    # Construct image file name
                    aPictureName = thePictureDir + aFieldName + "_" + str(timeValue) + "_DEFORMEDSHAPESCALARMAP." + thePictureExt

                    # Show and record the presentation
                    ProcessPrsForTest(aPrs, theView, aPictureName)

        if HasValue(thePrsTypeList, PrsTypeEnum.VECTORS):
            # Create  Vectors for all timestamps if possible
            aIsPossible = IfPossible(theProxy, aFieldEntity, aFieldName, PrsTypeEnum.VECTORS)
            if (aIsPossible):
                for timeStampNb in xrange(1, len(aTimeStamps)+1):
                    timeValue = aTimeStamps[timeStampNb-1]
                    print "          Creating Vectors on %s, time = %s... " % (aFieldName, str(timeValue)),
                    aPrs = VectorsOnField(theProxy, aFieldEntity, aFieldName, timeStampNb)
                    if aPrs is None :
                        print "Error: can't create Vectors." 
                    else:
                        print "OK" 

                    # Construct image file name
                    aPictureName = thePictureDir + aFieldName + "_" + str(timeValue) + "_VECTORS." + thePictureExt

                    # Show and record the presentation
                    ProcessPrsForTest(aPrs, theView, aPictureName)

    # Delete the proxy if necessary
    if (theIsAutoDelete):
        Delete(proxy=theProxy)
