"""
This module is intended to provide Python API for building presentations
typical for Post-Pro module (Scalar Map, Deformed Shape, Vectors, etc.)
"""


from __future__ import division
from __future__ import print_function

import os
import re
from math import sqrt
from string import upper

import pvsimple as pv

# Constants
EPS = 1E-3
FLT_MIN = 1E-37
VTK_LARGE_FLOAT = 1E+38
GAP_COEFFICIENT = 0.0001


# Enumerations
class PrsTypeEnum:
    """
    Post-Pro presentation types.
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
    GAUSSPOINTS = 10

    _type2name = {MESH: 'Mesh',
                  SCALARMAP: 'Scalar Map',
                  ISOSURFACES: 'Iso Surfaces',
                  CUTPLANES: 'Cut Planes',
                  CUTLINES: 'Cut Lines',
                  DEFORMEDSHAPE: 'Deformed Shape',
                  DEFORMEDSHAPESCALARMAP: 'Deformed Shape And Scalar Map',
                  VECTORS: 'Vectors',
                  PLOT3D: 'Plot3D',
                  STREAMLINES: 'Stream Lines',
                  GAUSSPOINTS: 'Gauss Points'}
    
    @classmethod
    def get_name(cls, type):
        """Return presentaion name by its type."""
        return cls._type2name[type]

class EntityType:
    """
    Entity types.
    """
    NODE = 0
    CELL = 1

    _type2name = {NODE: 'OnPoint',
                  CELL: 'OnCell'}
    
    _name2type = {'OnPoint': NODE,
                  'OnCell': CELL}

    @classmethod
    def get_name(cls, type):
        """Return entity name (used in full group names) by its type."""
        return cls._type2name[type]

    @classmethod
    def get_type(cls, name):
        """Return entity type by its name (used in full group names)."""
        return cls._name2type[name]

class Orientation:
    """
    Orientation types.
    """
    AUTO = 0
    XY = 1
    YZ = 2
    ZX = 3

class GlyphPos:
    """
    Glyph positions.
    """
    CENTER = 0
    TAIL = 1
    HEAD = 2

class GaussType:
    """
    Types of Gauss Points representation.
    """
    SPRITE = 0 	
    POINT = 1 	
    SPHERE = 2

    _type2mode = {SPRITE: 'Texture',
                  POINT: 'SimplePoint',
                  SPHERE: 'Sphere'}

    @classmethod
    def get_mode(cls, type):
        """Return paraview point sprite mode by the primitive type."""
        return cls._type2mode[type]

# Internal classes
class _aux_counter:
    "Internal class."
    first_render = True


def process_prs_for_test(thePrs, theView, thePictureName):
    """Show presentation and record image.
    
    Keyword arguments:
    thePrs -- the presentation to show
    theView -- the render view
    thePictureName -- the full name of a graphics file to save
    
    """
    # Show the presentation
    thePrs.Visibility = 1 
    
    # Reset the view 
    ResetView(theView) 
           
    # Create a directory for screenshot if necessary
    picture_name = re.sub("\s+","_", thePictureName)
    pic_dir = os.path.dirname(thePictureName)
    if not os.path.exists(pic_dir):
        os.makedirs(pic_dir)
        
    # Save picture
    pv.WriteImage(picture_name, view=theView, Magnification=1) 
    thePrs.Visibility = 0
    
    # Hide all
    HideAll(theView, True)

def ResetView(theView=None):
    """Reset the view.
    
    If the view is not passed, the active view is used.
    """
    if not theView:
        theView = pv.GetRenderView()

    if _aux_counter.first_render:
        # Camera preferences
        theView.CameraFocalPoint = [0.0, 0.0, 0.0]
        theView.CameraViewUp = [0.0, 0.0, 1.0]
        theView.CameraPosition = [738.946, -738.946, 738.946]

        # Turn on the headligth
        theView.LightSwitch = 1
        theView.LightIntensity = 0.5

        # Use parallel projection
        theView.CameraParallelProjection = 1
        
        _aux_counter.first_render = False

    theView.ResetCamera()
    pv.Render(view=theView)

def HideAll(theView, toRemove=False):
    """Hide all representations in the view."""
    if not theView:
        return
    aRepLst = theView.Representations
    for rep in aRepLst:
        if hasattr(rep, 'Visibility') and rep.Visibility != 0:
            rep.Visibility = 0
        if toRemove:
            theView.Representations.remove(rep)
    pv.Render(view=theView)

def DisplayOnly(theView, thePrs):
    """Display the presentation in the view.

    Hides all other presentations in the view.
    """
    HideAll(theView)
    if (hasattr(thePrs, 'Visibility') and thePrs.Visibility != 1):
        thePrs.Visibility = 1
    
def _check_vector_mode(theVectorMode, theNbComponents):
    """Check vector mode."""
    if theVectorMode not in ('Magnitude', 'X', 'Y', 'Z'):
        raise ValueError("Unexistent vector mode: {0}".format(theVectorMode))
    
    if ((theNbComponents == 1 and (theVectorMode=='Y' or theVectorMode=='Z')) or
        (theNbComponents == 2 and  theVectorMode=='Z')):
        raise ValueError("Incorrect vector mode {0} for {1}-component field".
                         format(theVectorMode, theNbComponents))

def GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode='Magnitude'):
    """Get data range for the field."""
    entity_data_info = None
    if (theEntityType == 'CELL_DATA'):
        entity_data_info = theProxy.GetCellDataInformation()
    else:
        entity_data_info = theProxy.GetPointDataInformation()

    data_range = []
    
    if (theVectorMode=='Magnitude'):
        data_range = entity_data_info[theFieldName].GetComponentRange(-1)
    elif (theVectorMode=='X'):
        data_range = entity_data_info[theFieldName].GetComponentRange(0)
    elif (theVectorMode=='Y'):
        data_range = entity_data_info[theFieldName].GetComponentRange(1)
    elif (theVectorMode=='Z'):
        data_range = entity_data_info[theFieldName].GetComponentRange(2)
    else:
        raise ValueError("Incorrect vector mode: {0}".format(theVectorMode))

    return data_range

def GetBounds(theProxy):
    """Get bounds of the proxy."""
    dataInfo = theProxy.GetDataInformation()
    bounds_info = dataInfo.GetBounds()

    return bounds_info

def GetXRange(theProxy):
    """Get X range."""
    bounds_info = GetBounds(theProxy)
    return bounds_info[0:2]

def GetYRange(theProxy):
    """Get Y range."""
    bounds_info = GetBounds(theProxy)
    return bounds_info[2:4]

def GetZRange(theProxy):
    """Get Z range."""
    bounds_info = GetBounds(theProxy)
    return bounds_info[4:6]

def IsPlanarInput(theProxy):
    """Check if the given input is planar."""
    bounds_info = GetBounds(theProxy)

    if ( abs(bounds_info[0] - bounds_info[1]) <= FLT_MIN or
         abs(bounds_info[2] - bounds_info[3]) <= FLT_MIN or
         abs(bounds_info[4] - bounds_info[5]) <= FLT_MIN ):
        return True

    return False

def IsDataOnCells(theProxy, theFieldName):
    """Check if the field data is on cells."""
    cell_data_info = theProxy.GetCellDataInformation()
    return (theFieldName in cell_data_info.keys())

def GetOrientation(theProxy):
    """Get the optimum cutting plane orientation for Plot3d."""
    orientation = Orientation.XY

    bounds = GetBounds(theProxy)
    delta = [bounds[1] - bounds[0],
              bounds[3] - bounds[2],
              bounds[5] - bounds[4] ]

    if (delta[0] >= delta[1] and delta[0] >= delta[2]):
        if (delta[1] >= delta[2]):
            orientation = Orientation.XY
        else:
            orientation = Orientation.ZX
    elif (delta[1] >= delta[0] and delta[1] >= delta[2]):
        if (delta[0] >= delta[2]):
            orientation = Orientation.XY
        else:
            orientation = Orientation.YZ
    elif (delta[2] >= delta[0] and delta[2] >= delta[1]):
        if (delta[0] >= delta[1]):
            orientation = Orientation.ZX
        else:
            orientation = Orientation.YZ

    return orientation

def GetNormalByOrientation(theOrientation):
    """Get normal for the plane by its orientation."""
    normal = [0.0, 0.0, 0.0]
    if (theOrientation == Orientation.XY):
        normal[2] = 1.0
    elif (theOrientation == Orientation.ZX):
        normal[1] = 1.0
    elif (theOrientation == Orientation.YZ):
        normal[0] = 1.0
        
    return normal

def GetRangeForOrientation(theProxy, theOrientation):
    """Get source range for cutting plane orientation."""
    val_range = []
    if (theOrientation == Orientation.XY):
        val_range = GetZRange(theProxy)
    elif (theOrientation == Orientation.ZX):
        val_range = GetYRange(theProxy)
    elif (theOrientation == Orientation.YZ):
        val_range = GetXRange(theProxy)

    return val_range

def GetPositions(val_range, theNbPlanes, theDisplacement):
    """Compute plane positions."""
    positions = []
    if (theNbPlanes > 1):
        aDistance = val_range[1] - val_range[0]
        val_range[1] = val_range[0] + (1.0 - EPS)*aDistance
        val_range[0] = val_range[0] + EPS*aDistance
        
        aDistance = val_range[1] - val_range[0]
        step = aDistance/(theNbPlanes-1)
        aDisplacement = step*theDisplacement
        startPos = val_range[0] - 0.5*step + aDisplacement
        
        for i in xrange(theNbPlanes):
            pos = startPos + i*step
            positions.append(pos)
    elif (theNbPlanes == 1):
        pos = val_range[0] + (val_range[1] - val_range[0])*theDisplacement
        positions.append(pos)
        
    return positions
    
def GetNbComponents(theProxy, theEntityType, theFieldName):
    """Return number of components for the field."""
    entity_data_info = None
    if (theEntityType == 'CELL_DATA'):
        entity_data_info = theProxy.GetCellDataInformation()
    else:
        entity_data_info = theProxy.GetPointDataInformation()
    nb_components = entity_data_info[theFieldName].GetNumberOfComponents()
    return nb_components

def GetScaleFactor(theProxy):
    """Compute scale factor."""
    data_info = theProxy.GetDataInformation()

    nb_cells = data_info.GetNumberOfCells()
    nb_points = data_info.GetNumberOfPoints()
    nb_elements = nb_cells
    if (nb_elements < 1):
        nb_elements = nb_points

    bounds = GetBounds(theProxy)

    volume = 1
    vol = dim = 0
    
    for i in xrange(0, 6, 2):
        vol = abs(bounds[i+1] - bounds[i])
        if (vol > 0):
            dim += 1
            volume *= vol

    if (nb_elements == 0 or dim < 1):
        return 0

    volume /= nb_elements
    return pow(volume, 1.0/dim)
            
def GetDefaultScale(thePrsType, theProxy, theEntityType, theFieldName):
    """Get default scale factor."""
    data_range = GetDataRange(theProxy, theEntityType, theFieldName)
    
    if thePrsType == PrsTypeEnum.DEFORMEDSHAPE:
        EPS = 1.0 / VTK_LARGE_FLOAT
        if ( abs(data_range[1]) > EPS ):
            scale_factor = GetScaleFactor(theProxy)
            return scale_factor / data_range[1]
    elif thePrsType == PrsTypeEnum.PLOT3D:
        bounds = GetBounds(theProxy)
        length = sqrt( (bounds[1]-bounds[0])**2 +
                        (bounds[3]-bounds[2])**2 +
                        (bounds[5]-bounds[4])**2 )
        
        EPS = 0.3
        if data_range[1] > 0:
            return length / data_range[1] * EPS
        
    return 0

def GetCalcMagnitude(theProxy, theAttributeMode, theArrayName):
    """Compute magnitude for the given vector array via Calculator.
    
    Return the calculator object.
    """
    calculator = None
    
    # Transform vector array to scalar array if possible
    nb_components = GetNbComponents(theProxy, upper(theAttributeMode), theArrayName)
    if (nb_components > 1):
        calculator = pv.Calculator(theProxy)
        calculator.AttributeMode = theAttributeMode
        if (nb_components == 2):
            # Workaroud: calculator unable to compute magnitude
            # if number of components equal to 2
            calculator.Function = "sqrt({0}_X^2+{0}_Y^2)".format(theArrayName)
        else:
            calculator.Function = "mag({0})".format(theArrayName)
        calculator.ResultArrayName = theArrayName + "_magnitude"
        calculator.UpdatePipeline()
            
    return calculator

def IfPossible(theProxy, theFieldName, theEntityType, thePrsType):
    """Check if the presentation is possible on the given field."""
    result = True
    if (thePrsType == PrsTypeEnum.DEFORMEDSHAPE or
        thePrsType == PrsTypeEnum.DEFORMEDSHAPESCALARMAP or
        thePrsType == PrsTypeEnum.VECTORS or
        thePrsType == PrsTypeEnum.STREAMLINES):
        nbComp = GetNbComponents(theProxy, theEntityType, theFieldName)
        result = (nbComp > 1)
    elif (thePrsType == PrsTypeEnum.GAUSSPOINTS):
        result = (theEntityType == "CELL_DATA")
    elif (thePrsType == PrsTypeEnum.MESH):
        result = len(_get_group_names(theProxy, theFieldName, theEntityType)) > 0
                
    return result

def GetStdScalarBar(theTitle, theLookupTable):
    """Create scalar bar."""
    scalarBar = pv.CreateScalarBar(Enabled=1, LabelFontSize=10, TitleFontSize=8,
                                   NumberOfLabels=5)
    scalarBar.Title = theTitle
    scalarBar.LookupTable = theLookupTable
    return scalarBar

def GetStdLookupTable(theFieldName, theNbComp, theVectorMode='Magnitude'):
    """Get lookup table for the given field."""
    lookup_table = pv.GetLookupTableForArray(theFieldName, theNbComp)

    if (theVectorMode=='Magnitude'):
        lookup_table.VectorMode=theVectorMode
    elif (theVectorMode=='X'):
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 0
    elif (theVectorMode=='Y'):
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 1
    elif (theVectorMode=='Z'):
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 2
    else:
        raise ValueError("Incorrect vector mode: {0}".format(theVectorMode))
        
    lookup_table.Discretize = 0
    lookup_table.ColorSpace = 'HSV'
    lookup_table.LockScalarRange = 0
    
    return lookup_table

def _get_group_mesh_name(full_group_name):
    """Return mesh name of the group by its full name."""
    group_name = full_group_name.split('/')[1]

    return group_name

def _get_group_entity(full_group_name):
    """Return entity type of the group by its full name."""
    entity = full_group_name.split('/')[2]

    entity_type = EntityType.get_type(entity)
    
    return entity_type

def _get_mesh_names(proxy):
    """Return all mesh names in the given proxy as a set."""
    groups = proxy.Groups.Available
    mesh_names = set([_get_group_mesh_name(item) for item in groups])
    
    return mesh_names

def _get_group_names(proxy, mesh_name, entity_type):
    """Return full names of all groups of the given entity type
    from the mesh with the given name as a list.
    """
    groups = proxy.Groups.Available

    condition = lambda item: _get_group_mesh_name(item) == mesh_name and\
                _get_group_entity(item) == entity_type
    group_names = [item for item in groups if condition(item)]
    
    return group_names

def GetTime(theProxy, theTimeStampNumber):
    """Get time value by timestamp number."""
    # Check timestamp number
    timestamps = theProxy.TimestepValues.GetData()
    if ((theTimeStampNumber - 1) not in xrange(len(timestamps))):
        raise ValueError("Timestamp number is out of range: {0}".format(theTimeStampNumber))

    # Return time value
    return timestamps[theTimeStampNumber-1]
    
def ScalarMapOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                     theVectorMode='Magnitude'):
    """Creates Scalar Map presentation on the field.

    Keyword arguments:
    theProxy -- the pipeline object, containig data
    theEntityType -- the entity type
    theFieldName -- the field name
    theTimeStampNumber -- the number of time step (1, 2, ...)
    theVectorMode -- the mode of transformation of vector values into scalar values,
    applicable only if the field contains vector values.
    Possible modes: 'Magnitude' - vector module; 'X', 'Y', 'Z' - vector components.
    
    """
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)
    
    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)

    # Show pipeline object
    scalarmap = pv.GetRepresentation(theProxy)
   
    # Get lookup table
    lookup_table = GetStdLookupTable(theFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    scalarmap.ColorAttributeType = theEntityType
    scalarmap.ColorArrayName = theFieldName
    scalarmap.LookupTable = lookup_table
    # Commented by @MZN:
    #if (not IsPlanarInput(theProxy)):
    #    scalarmap.BackfaceRepresentation = 'Cull Frontface'

    # Add scalar bar
    bar_title = theFieldName + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)
    
    return scalarmap

def CutPlanesOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                     theNbPlanes=10, theOrientation = Orientation.YZ, theDisplacement=0.5,
                     theVectorMode='Magnitude'):
    """Creates cut planes on the given field."""
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0
    
    # Create slice filter
    slice = pv.Slice(theProxy)
    slice.SliceType="Plane"

    # Set cut planes normal and get range
    normal = [0.0, 0.0, 0.0]
    val_range = []
    if (theOrientation == Orientation.XY):
        normal[2] = 1.0
        val_range = GetZRange(theProxy)
    elif (theOrientation == Orientation.ZX):
        normal[1] = 1.0
        val_range = GetYRange(theProxy)
    elif (theOrientation == Orientation.YZ):
        normal[0] = 1.0
        val_range = GetXRange(theProxy)
        
    slice.SliceType.Normal = normal

    # Set cut planes positions
    slice.SliceOffsetValues = GetPositions(val_range, theNbPlanes, theDisplacement)
    cut_planes = pv.GetRepresentation(slice)
   
    # Get lookup table
    lookup_table = GetStdLookupTable(theFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    cut_planes.ColorAttributeType = theEntityType
    cut_planes.ColorArrayName = theFieldName
    cut_planes.LookupTable = lookup_table

    # Add scalar bar
    bar_title = theFieldName + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)
    
    return cut_planes

def CutLinesOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                    theNbLines=10,
                    theOrientation1 = Orientation.XY, theOrientation2 = Orientation.YZ,
                    theDisplacement1=0.5, theDisplacement2=0.5,
                    theVectorMode='Magnitude'):
    """Creates cut lines on the given field."""
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Create base plane
    base_plane = pv.Slice(theProxy)
    base_plane.SliceType="Plane"

    # Set base plane normal and get position
    base_normal = [0.0, 0.0, 0.0]
    valrange_base = []
    if (theOrientation1 == Orientation.XY):
        base_normal[2] = 1.0
        valrange_base = GetZRange(theProxy)
    elif (theOrientation1 == Orientation.ZX):
        base_normal[1] = 1.0
        valrange_base = GetYRange(theProxy)
    elif (theOrientation1 == Orientation.YZ):
        if (theOrientation2 == theOrientation1):
            theOrientation2 = Orientation.ZX
        base_normal[0] = 1.0
        valrange_base = GetXRange(theProxy)
        
    base_plane.SliceType.Normal = base_normal

    # Set base plane position
    base_plane.SliceOffsetValues = GetPositions(valrange_base, 1, theDisplacement1)

    # Check base plane
    base_plane.UpdatePipeline()
    if (base_plane.GetDataInformation().GetNumberOfCells() == 0):
        base_plane = theProxy
    
    # Create cutting planes
    cut_planes = pv.Slice(base_plane)
    cut_planes.SliceType="Plane"

    # Set cutting planes normal and get positions
    cut_normal = [0.0, 0.0, 0.0]
    valrange_cut = []
    if (theOrientation2 == Orientation.XY):
        cut_normal[2] = 1.0
        valrange_cut = GetZRange(theProxy)
    elif (theOrientation2 == Orientation.ZX):
        cut_normal[1] = 1.0
        valrange_cut = GetYRange(base_plane)
    elif (theOrientation2 == Orientation.YZ):
        cut_normal[0] = 1.0
        valrange_cut = GetXRange(base_plane)
        
    cut_planes.SliceType.Normal = cut_normal

    # Set cutting planes position
    cut_planes.SliceOffsetValues = GetPositions(valrange_cut, theNbLines, theDisplacement2)
    cut_lines = pv.GetRepresentation(cut_planes)
   
    # Get lookup table
    lookup_table = GetStdLookupTable(theFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]
    
    # Set properties
    cut_lines.ColorAttributeType = theEntityType
    cut_lines.ColorArrayName = theFieldName
    cut_lines.LookupTable = lookup_table

    # Set wireframe represenatation mode
    cut_lines.Representation = 'Wireframe'

    # Add scalar bar
    bar_title = theFieldName + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)

    return cut_lines

def VectorsOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber, theScaleFactor=-1,
                   theGlyphPos=GlyphPos.TAIL, theIsColored=False, theVectorMode='Magnitude'):
    """Creates vectors on the given field."""
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)

    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0
    
    # Cell centers
    if (IsDataOnCells(theProxy, theFieldName)):
        cell_centers = pv.CellCenters(theProxy)
        cell_centers.VertexCells = 1

    # Vectors
    glyph = pv.Glyph()
    glyph.Vectors = theFieldName
    glyph.ScaleMode = 'vector'
    glyph.MaskPoints = 0

    glyph.GlyphType = "2D Glyph"
    glyph.GlyphType.GlyphType = 'Arrow'
    if (theGlyphPos == GlyphPos.TAIL):
        glyph.GlyphType.Center = [0.5, 0.0, 0.0]
    elif (theGlyphPos == GlyphPos.HEAD):
        glyph.GlyphType.Center = [-0.5, 0.0, 0.0]
    elif (theGlyphPos == GlyphPos.CENTER):
        glyph.GlyphType.Center = [0.0, 0.0, 0.0]
        
    if (theScaleFactor > 0):
        glyph.SetScaleFactor = theScaleFactor
    else:
        def_scale = GetDefaultScale(PrsTypeEnum.DEFORMEDSHAPE, theProxy, theEntityType, theFieldName)
        #@MZN DEBUG
        print("DEFAULT SCALE FACTOR = ", def_scale)
        glyph.SetScaleFactor = def_scale

    glyph.UpdatePipeline()

    vectors = pv.GetRepresentation(glyph)
   
    # Get lookup table
    lookup_table = GetStdLookupTable(theFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if (theIsColored):
        vectors.ColorArrayName = 'GlyphVector'
    else:
        vectors.ColorArrayName = ''
    vectors.LookupTable = lookup_table
    
    vectors.LineWidth = 1.0

    # Set wireframe represenatation mode
    vectors.Representation = 'Wireframe'

    # Add scalar bar
    bar_title = theFieldName + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)
    
    return vectors

def DeformedShapeOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber, theScaleFactor=-1,
                         theIsColored=False, theVectorMode='Magnitude'):
    """Creates Defromed shape on the given field."""
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)

    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    merge_blocks = pv.MergeBlocks(theProxy)
    
    # Cell data to point data
    if (IsDataOnCells(theProxy, theFieldName)):
        cell_to_point = pv.CellDatatoPointData()
        cell_to_point.PassCellData = 1
        
    # Warp by vector
    warp_vector = pv.WarpByVector()
    warp_vector.Vectors = [theFieldName]
    if (theScaleFactor > 0):
        warp_vector.ScaleFactor = theScaleFactor
    else:
        def_scale = GetDefaultScale(PrsTypeEnum.DEFORMEDSHAPE, theProxy, theEntityType, theFieldName)
        #@MZN DEBUG
        print("DEFAULT SCALE FACTOR = ", def_scale)
        warp_vector.ScaleFactor = def_scale
        
    defshape = pv.GetRepresentation(warp_vector)
   
    # Get lookup table
    lookup_table = GetStdLookupTable(theFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if (theIsColored):
        defshape.ColorAttributeType = theEntityType
        defshape.ColorArrayName = theFieldName
    else:
        defshape.ColorArrayName = ''
    defshape.LookupTable = lookup_table

    # Set wireframe represenatation mode
    defshape.Representation = 'Wireframe'

    # Add scalar bar
    bar_title = theFieldName + ", " + str(time_value)
    if nb_components > 1:
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)
    
    return defshape

def DeformedShapeAndScalarMapOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber, theScaleFactor=-1,
                                     theScalarEntityType=None, theScalarFieldName=None,
                                     theVectorMode='Magnitude'):
    """Creates Defromed shape And Scalar Map on the given field."""
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Set scalar field by default
    if ((theScalarEntityType is None) or (theScalarFieldName is None)):
        theScalarEntityType = theEntityType
        theScalarFieldName = theFieldName

    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    merge_blocks = pv.MergeBlocks(theProxy)

    # Cell data to point data
    if (IsDataOnCells(theProxy, theFieldName)):
        cell_to_point = pv.CellDatatoPointData()
        cell_to_point.PassCellData = 1
        
    # Warp by vector
    warp_vector = pv.WarpByVector()
    warp_vector.Vectors = [theFieldName]
    if (theScaleFactor > 0):
        warp_vector.ScaleFactor = theScaleFactor
    else:
        def_scale = GetDefaultScale(PrsTypeEnum.DEFORMEDSHAPE, theProxy, theEntityType, theFieldName)
        #@MZN DEBUG
        print("DEFAULT SCALE FACTOR = ", def_scale)
        warp_vector.ScaleFactor = def_scale
    
    defshapeandmap = pv.GetRepresentation(warp_vector)
   
    # Get lookup table
    lookup_table = GetStdLookupTable(theScalarFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theScalarEntityType, theScalarFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]
    
    # Set properties
    defshapeandmap.ColorArrayName = theScalarFieldName
    defshapeandmap.LookupTable = lookup_table
    defshapeandmap.ColorAttributeType = theScalarEntityType
     
    # Add scalar bar
    bar_title = theScalarFieldName + ", " + str(time_value)
    nb_components = GetNbComponents(theProxy, theScalarEntityType, theScalarFieldName)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)
       
    return defshapeandmap

def Plot3DOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                  theOrientation = Orientation.AUTO, thePosition=0.5, theIsRelative=True,
                  theScaleFactor=-1, theIsContourPrs=False, theNbOfContours=32,
                  theVectorMode='Magnitude'):
    """Creates plot 3d on the given field."""
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    merge_blocks = pv.MergeBlocks(theProxy)

    poly_data = None

    # Cutting plane
    
    # Define orientation if necessary (auto mode)
    orientation = theOrientation
    if (orientation == Orientation.AUTO):
        orientation = GetOrientation(theProxy)

    # Get cutting plane normal
    normal = GetNormalByOrientation(orientation)
      
    if (not IsPlanarInput(theProxy)):
        # Create slice filter
        slice=Slice(merge_blocks)
        slice.SliceType="Plane"

        # Set cutting plane normal
        slice.SliceType.Normal = normal

        # Set cutting plane position
        val_range = GetRangeForOrientation(theProxy, orientation)
        
        if (theIsRelative):
            slice.SliceOffsetValues = GetPositions(val_range, 1, thePosition)
        else:
            slice.SliceOffsetValues = thePosition

        slice.UpdatePipeline()
        poly_data = slice

    use_normal = 0
    # Geometry filter
    if (not poly_data or poly_data.GetDataInformation().GetNumberOfCells() == 0):
        geometry_filter = pv.GeometryFilter(merge_blocks)
        poly_data = geometry_filter
        use_normal = 1 #MZN: temporary workaround
    
    warp_scalar = None
    plot3d = None
    source = poly_data
    
    if (IsDataOnCells(poly_data, theFieldName)):
        # Cell data to point data
        cell_to_point = pv.CellDatatoPointData(poly_data)
        cell_to_point.PassCellData = 1
        source = cell_to_point
        
    scalars = ['POINTS', theFieldName]
    
    # Transform vector array to scalar array if necessary
    if (nb_components > 1):
        calculator = GetCalcMagnitude(source, "point_data", theFieldName)
        scalars = ['POINTS', calculator.ResultArrayName]
        source = calculator
    
    # Warp by scalar
    warp_scalar = pv.WarpByScalar(source)
    warp_scalar.Scalars = scalars
    warp_scalar.Normal = normal
    warp_scalar.UseNormal = use_normal
    if (theScaleFactor > 0):
        warp_scalar.ScaleFactor = theScaleFactor
    else:
        def_scale = GetDefaultScale(PrsTypeEnum.PLOT3D, theProxy, theEntityType, theFieldName)
        warp_scalar.ScaleFactor = def_scale
        
    warp_scalar.UpdatePipeline()
    
    if (theIsContourPrs):
        # Contours
        contour = pv.Contour(warp_scalar)
        contour.PointMergeMethod = "Uniform Binning"
        contour.ContourBy = ['POINTS', theFieldName]
        scalar_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
        contour.Isosurfaces = GetPositions(scalar_range, theNbOfContours, 0)
        contour.UpdatePipeline()
        plot3d = pv.GetRepresentation(contour)
    else:
        plot3d = pv.GetRepresentation(warp_scalar)
   
    # Get lookup table
    lookup_table = GetStdLookupTable(theFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    plot3d.ColorAttributeType = theEntityType
    plot3d.ColorArrayName = theFieldName
    plot3d.LookupTable = lookup_table
  
    # Add scalar bar
    bar_title = theFieldName + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)

    return plot3d

def IsoSurfacesOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                       theRange = None, theNbOfSurfaces=10, theIsColored = True,
                       theColor = None, theVectorMode='Magnitude'):
    """Creates Iso Surfaces on the given field."""
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)
    
    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0

    # Do merge
    merge_blocks = pv.MergeBlocks(theProxy)
    source = merge_blocks

    # Transform cell data into point data if necessary
    if (IsDataOnCells(theProxy, theFieldName)):
        cell_to_point = pv.CellDatatoPointData(source)
        cell_to_point.PassCellData = 1
        source = cell_to_point
        
    contour_by = ['POINTS', theFieldName]

    # Transform vector array to scalar array if necessary
    if (nb_components > 1):
        calculator = GetCalcMagnitude(source, "point_data", theFieldName)
	contour_by = ['POINTS', calculator.ResultArrayName]
        source = calculator

    # Contour filter settings
    contour = pv.Contour(source)
    contour.ComputeScalars = 1
    contour.ContourBy = contour_by

    # Specify the range
    scalar_range = theRange
    if (scalar_range is None):
        scalar_range = GetDataRange(theProxy, theEntityType, theFieldName)
        #@MZN: debug
        print("scalar_range[0] = ", scalar_range[0])
        print("scalar_range[1] = ", scalar_range[1])
        # Cut off the range
        if (scalar_range[0] <= scalar_range[1]):
            delta = abs(scalar_range[1] - scalar_range[0]) * GAP_COEFFICIENT
            scalar_range[0] += delta
            scalar_range[1] -= delta

    # Calculate contour values for the range
    surfaces = []
    for i in xrange(theNbOfSurfaces):
        pos = scalar_range[0] + i*(scalar_range[1] - scalar_range[0])/(theNbOfSurfaces - 1)
        surfaces.append(pos)

    # Set contour values
    contour.Isosurfaces = surfaces
    
    # Get Iso Surfaces representation
    isosurfaces = pv.GetRepresentation(contour)

    # Get lookup table
    lookup_table = GetStdLookupTable(theFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theEntityType, theFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set display properties
    if (theIsColored):
        isosurfaces.ColorAttributeType = theEntityType
        isosurfaces.ColorArrayName = theFieldName
    else:
        isosurfaces.ColorArrayName = ''
        if (theColor is not None):
            isosurfaces.DiffuseColor = theColor
    isosurfaces.LookupTable = lookup_table
  
    # Add scalar bar
    bar_title = theFieldName + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)

    return isosurfaces

def GaussPointsOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber,
                       theIsDeformed = False, theScaleFactor=-1,
                       theIsColored = True, theColor = None,
                       thePrimitiveType = GaussType.SPRITE, theVectorMode='Magnitude'):
    """Creates Gauss points on the given field.
    
    If scale factor is defined deformation will be applied.
    """
    # Check vector mode
    nb_components = GetNbComponents(theProxy, theEntityType, theFieldName)
    _check_vector_mode(theVectorMode, nb_components)
    
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0

    # Quadrature point arrays
    qp_arrays = theProxy.QuadraturePointArrays

    # If no gauss points are defined, use cell centers
    source = None
    if qp_arrays:
        generate_qp = pv.GenerateQuadraturePoints(theProxy)
        generate_qp.SelectSourceArray = ['CELLS', theFieldName] #@MZN
        source = generate_qp
    else:
        cell_centers = pv.CellCenters(theProxy)
        cell_centers.VertexCells = 1 #@MZN: necessary?
        source = cell_centers

    source.UpdatePipeline()

    # Check if deformation enabled
    if (theIsDeformed and (nb_components > 1)):
        # Warp by vector
        warp_vector = pv.WarpByVector(source)
        warp_vector.Vectors = [theFieldName]
        if (theScaleFactor > 0):
            warp_vector.ScaleFactor = theScaleFactor
        else:
            def_scale = GetDefaultScale(PrsTypeEnum.DEFORMEDSHAPE, theProxy, theEntityType, theFieldName)
            warp_vector.ScaleFactor = def_scale
        warp_vector.UpdatePipeline()
        source = warp_vector

    # Point sprite settings
    radius_array = theFieldName
    # Transform vector array to scalar array if necessary
    if (nb_components > 1):
        on_cells = IsDataOnCells(theProxy, theFieldName)
        data_type = "cell_data" if on_cells else "point_data"
        calculator = GetCalcMagnitude(source, data_type, theFieldName)
        radius_array = calculator.ResultArrayName
        source = calculator
    array_range = GetDataRange(theProxy, theEntityType, radius_array)
    #@MZN: Add magnification etc.
    gausspnt = pv.GetRepresentation(source)
    gausspnt.Representation = 'Point Sprite'
    gausspnt.RenderMode = GaussType.get_mode(thePrimitiveType)
    gausspnt.RadiusTransferFunctionEnabled = 1
    gausspnt.RadiusMode = 'Scalar'
    gausspnt.RadiusScalarRange = array_range
    gausspnt.RadiusArray = [None, radius_array]
    gausspnt.RadiusIsProportional = 1
    
    # Get lookup table
    lookup_table = GetStdLookupTable(theScalarFieldName, nb_components, theVectorMode)

    # Set field range if necessary
    data_range = GetDataRange(theProxy, theScalarEntityType, theScalarFieldName, theVectorMode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]
    
    # Set display properties
    if (theIsColored):
        gausspnt.ColorAttributeType = theEntityType
        gausspnt.ColorArrayName = theFieldName
    else:
        gausspnt.ColorArrayName = ''
        if (theColor is not None):
            gausspnt.DiffuseColor = theColor
            
    gausspnt.LookupTable = lookup_table
    
    # Add scalar bar
    bar_title = theScalarFieldName + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + theVectorMode
    scalarBar = GetStdScalarBar(bar_title, lookup_table)
    pv.GetRenderView().Representations.append(scalarBar)

    return gausspnt

def StreamLinesOnField(theProxy, theEntityType, theFieldName, theTimeStampNumber, theScaleFactor=-1,
                       theIsColored=False, theVectorMode='Magnitude'):
    """Creates Stream Lines on the given field."""
    # Get time value
    time_value = GetTime(theProxy, theTimeStampNumber)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, theProxy)
    
    # Hide initial object
    rep = pv.GetRepresentation(theProxy)
    rep.Visibility = 0

    # Cell centers
    if (IsDataOnCells(theProxy, theFieldName)):
        cell_centers = pv.CellCenters(theProxy)
        cell_centers.VertexCells = 1
    
    streamlines = None
    
    return streamlines

def MeshOnEntity(theProxy, theMeshName, theEntityType):
    """Creates submesh of the entity type for the mesh.

    Keyword arguments:
    theProxy -- the pipeline object, containig data
    theMeshName -- the mesh name
    theEntityType -- the entity type
    
    """
    # Get subset of groups on the given entity
    subset = _get_group_names(theProxy, theMeshName, theEntityType)

    # Select only groups of the given entity type
    theProxy.Groups = subset
    theProxy.UpdatePipeline()

    # Get representation object if the submesh is not empty
    prs = None
    if (theProxy.GetDataInformation().GetNumberOfPoints() or
        theProxy.GetDataInformation().GetNumberOfCells()):
        prs = pv.GetRepresentation(theProxy)
    
    return prs

def MeshOnGroup(theProxy, theGroupName):
    """Creates submesh on the group.

    Keyword arguments:
    theProxy -- the pipeline object, containig data
    theGroupName -- the full group name
    
    """
    # Select only the group with the given name
    #@MZN: DEBUG
    print ("theProxy.Groups = ", [theGroupName])
    one_group = [theGroupName]
    theProxy.Groups = one_group
    theProxy.UpdatePipeline()
    
    # Get representation object if the submesh is not empty
    prs = None

    #@MZN: DEBUG
    print ("theProxy.GetDataInformation().GetNumberOfPoints() = ", theProxy.GetDataInformation().GetNumberOfPoints())
    print ("theProxy.GetDataInformation().GetNumberOfCells() = ", theProxy.GetDataInformation().GetNumberOfCells())
    print ("theProxy.Groups.GetData() = : ", theProxy.Groups.GetData())  #@MZN

    # Check if the group was set
    if theProxy.Groups.GetData() == one_group:
        group_entity = _get_group_entity(theGroupName)
        # Check if the submesh is not empty
        nb_nodes = theProxy.GetDataInformation().GetNumberOfPoints()
        nb_cells = theProxy.GetDataInformation().GetNumberOfCells()
        on_nodes = group_entity == EntityType.NODE
        on_cells = group_entity == EntityType.CELL
        if (on_nodes and nb_points) or (on_cells and nb_cells):
            prs = pv.GetRepresentation(theProxy)
        
    return prs

def CreatePrsForFile(theParavis, theFileName, thePrsTypeList, thePictureDir,
                     thePictureExt, theIsAutoDelete = 0):
    """Build presentations of the given types for all fields of the given file."""
    print("Import {0}...".format(theFileName.split('/')[-1]), end='')
    theParavis.ImportFile(theFileName)
    proxy = pv.GetActiveSource()
    if proxy is None:
        print("FAILED")
        raise RuntimeError("Error: can't import file.")
    else: print("OK")

    # Get view
    view = pv.GetRenderView()

    # Create required presentations for the proxy
    CreatePrsForProxy(proxy, view, thePrsTypeList, thePictureDir, thePictureExt, theIsAutoDelete)

def _create_prs(thePrsType, theProxy, theFieldEntity, theFieldName, theTimeStampNb):
    """Internal method. Build presentation of the given type on the given field and
    timestamp number. Set the presentation properties like visu.CreatePrsForResult() do.
    """
    prs = None
    
    if thePrsType == PrsTypeEnum.SCALARMAP:
        prs = ScalarMapOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    elif thePrsType == PrsTypeEnum.CUTPLANES:
        prs = CutPlanesOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb,
                               theOrientation = Orientation.ZX)
    elif thePrsType == PrsTypeEnum.CUTLINES:
        prs = CutLinesOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb,
                              theOrientation1 = Orientation.XY, theOrientation2 = Orientation.ZX)
    elif thePrsType == PrsTypeEnum.DEFORMEDSHAPE:
        prs = DeformedShapeAndScalarMapOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    elif thePrsType == PrsTypeEnum.DEFORMEDSHAPESCALARMAP:
        prs = DeformedShapeAndScalarMapOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    elif thePrsType == PrsTypeEnum.VECTORS:
        prs = VectorsOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    elif thePrsType == PrsTypeEnum.PLOT3D:
        prs = Plot3DOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    elif thePrsType == PrsTypeEnum.ISOSURFACES:
        prs = IsoSurfacesOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    elif thePrsType == PrsTypeEnum.GAUSSPOINTS:
        prs = GaussPointsOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    elif thePrsType == PrsTypeEnum.STREAMLINES:
        prs = StreamLinesOnField(theProxy, theFieldEntity, theFieldName, theTimeStampNb)
    else:
        raise ValueError("Unexistent presentation type.")
        
    return prs
        
def CreatePrsForProxy(theProxy, theView, thePrsTypeList, thePictureDir, thePictureExt, theIsAutoDelete):
    """Build presentations of the given types for all fields of the given proxy.
    Save snapshots in graphics files (type depends on the given extension).
    Stores the files in the given directory.
    
    Keyword arguments:
    theProxy -- the pipeline object, containig data arrays
    theView  -- the render view
    thePrsTypeList  -- the list of presentation types to build
    thePictureDir   -- the directory path for saving snapshots
    thePictureExt   -- graphics files extension (determines file type)
    
    """
    
    field_names = theProxy.PointArrays.GetData()
    nb_on_nodes = len(field_names)
    field_names.extend(theProxy.CellArrays.GetData())

    # Add path separator to the end of picture path if necessery
    if not thePictureDir.endswith(os.sep):
        thePictureDir += os.sep
        
    # Mesh Presentation
    if PrsTypeEnum.MESH in thePrsTypeList:
        # Create Mesh presentation. Build all possible submeshes.

        # Remember the current state
        groups = list(theProxy.Groups)

        # Iterate on meshes
        mesh_names = _get_mesh_names(theProxy)
        for mesh_name in mesh_names:
            # Build mesh on nodes and cells
            for entity_type in (EntityType.NODE, EntityType.CELL):
                entity_name = EntityType.get_name(entity_type)
                if IfPossible(theProxy, mesh_name, entity_type, PrsTypeEnum.MESH):
                    print("Creating submesh on {0} for '{1}' mesh... ".format(entity_name,
                                                                              mesh_name),
                          end='')
                    prs = MeshOnEntity(theProxy, mesh_name, entity_type)
                    if prs is None :
                        print("FAILED")
                        continue
                    else:
                        print("OK")
                    # Construct image file name
                    pic_name = "{folder}{mesh}_{type}.{ext}".format(folder=thePictureDir,
                                                                    mesh=mesh_name,
                                                                    type=entity_name,
                                                                    ext=thePictureExt)

                    # Show and dump the presentation into a graphics file
                    process_prs_for_test(prs, theView, pic_name)
                    
                # Build submesh on all groups of the mesh
                mesh_groups = _get_group_names(theProxy, mesh_name, entity_type)
                for group in mesh_groups:
                    print("Creating submesh on group {0}... ".format(group), end='')
                    prs = MeshOnGroup(theProxy, group)
                    if prs is None :
                        print("FAILED")
                        continue
                    else:
                        print("OK")
                    # Construct image file name
                    pic_name = "{folder}{group}.{ext}".format(folder=thePictureDir,
                                                              group=group.replace('/','_'),
                                                              ext=thePictureExt)
                
                    # Show and dump the presentation into a graphics file
                    process_prs_for_test(prs, theView, pic_name)
                
        # Restore the state
        theProxy.Groups = groups
        theProxy.UpdatePipeline()
 
    # Presentations on fields
    for (i, field_name) in enumerate(field_names):
        # Select only the current field: necessary for getting the right timestamps
        field_entity = None
        if (i >= nb_on_nodes):
            field_entity = 'CELL_DATA'
            theProxy.PointArrays.DeselectAll()
            theProxy.CellArrays = [field_name]
        else:
            field_entity = 'POINT_DATA'
            theProxy.CellArrays.DeselectAll()
            theProxy.PointArrays = [field_name]

        # Get timestamps
        theProxy.UpdatePipeline()
        timestamps = theProxy.TimestepValues.GetData()
    
        for prs_type in thePrsTypeList:
            # Ignore mesh presentation
            if prs_type == PrsTypeEnum.MESH:
                continue
            
            # Get name of presentation type
            prs_name = PrsTypeEnum.get_name(prs_type)

            # Build the presentation if possible
            possible = IfPossible(theProxy, field_name, field_entity, prs_type)
            if possible:
                # Presentation type for graphics file name
                f_prs_type = prs_name.replace(' ', '').upper()
                
                for timestamp_nb in xrange(1, len(timestamps)+1):
                    time = timestamps[timestamp_nb-1]
                    print("Creating {0} on {1}, time = {2}... ".format(prs_name, field_name, time),
                          end='')
                    prs = _create_prs(prs_type, theProxy, field_entity, field_name, timestamp_nb)
                    if prs is None :
                        print("FAILED")
                        continue
                    else:
                        print("OK")
            
                    # Construct image file name
                    pic_name = "{folder}{field}_{time}_{type}.{ext}".format(folder=thePictureDir,
                                                                            field=field_name,
                                                                            time=time,
                                                                            type=f_prs_type,
                                                                            ext=thePictureExt)

                    # Show and dump the presentation into a graphics file
                    process_prs_for_test(prs, theView, pic_name)
