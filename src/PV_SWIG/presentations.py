"""
This module is intended to provide Python API for building presentations
typical for Post-Pro module (Scalar Map, Deformed Shape, Vectors, etc.)
"""


from __future__ import division
from __future__ import print_function

import os
import re
import warnings
from math import sqrt
from string import upper

import pvsimple as pv
# TODO(MZN): to be removed (issue with Point Sprite texture)
import paravisSM as sm


# Constants
EPS = 1E-3
FLT_MIN = 1E-37
VTK_LARGE_FLOAT = 1E+38
GAP_COEFFICIENT = 0.0001


# Globals
_current_bar = None


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

    _type2pvtype = {NODE: 'POINT_DATA',
                    CELL: 'CELL_DATA'}

    @classmethod
    def get_name(cls, type):
        """Return entity name (used in full group names) by its type."""
        return cls._type2name[type]

    @classmethod
    def get_type(cls, name):
        """Return entity type by its name (used in full group names)."""
        return cls._name2type[name]

    @classmethod
    def get_pvtype(cls, type):
        """Return entity type from ['CELL_DATA', 'POINT_DATA']"""
        return cls._type2pvtype[type]


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
class _AuxCounter:
    "Internal class."
    first_render = True


# Auxiliary functions
def process_prs_for_test(prs, view, picture_name, show_bar=True):
    """Show presentation and record image.

    Arguments:

    prs -- the presentation to show
    view -- the render view
    picture_name -- the full name of a graphics file to save
    show_bar -- to show scalar bar or not

    """
    # Show the presentation only
    display_only(prs, view)

    # Show scalar bar
    if show_bar and _current_bar:
        _current_bar.Visibility = 1

    # Reset the view
    reset_view(view)

    # Create a directory for screenshot if necessary
    file_name = re.sub("\s+", "_", picture_name)
    pic_dir = os.path.dirname(picture_name)
    if not os.path.exists(pic_dir):
        os.makedirs(pic_dir)

    # Save picture
    pv.WriteImage(file_name, view=view, Magnification=1)


def reset_view(view=None):
    """Reset the view.

    If the view is not passed, the active view is used.
    """
    if not view:
        view = pv.GetRenderView()

    #@MZN
    if True or _AuxCounter.first_render:
        # Camera preferences
        view.CameraFocalPoint = [0.0, 0.0, 0.0]
        view.CameraViewUp = [0.0, 0.0, 1.0]
        view.CameraPosition = [738.946, -738.946, 738.946]

        # Turn on the headligth
        view.LightSwitch = 1
        view.LightIntensity = 0.5

        # Use parallel projection
        view.CameraParallelProjection = 1

        _AuxCounter.first_render = False

    view.ResetCamera()
    pv.Render(view=view)


def hide_all(view, to_remove=False):
    """Hide all representations in the view."""
    if not view:
        view = pv.GetRenderView()

    rep_list = view.Representations
    for rep in rep_list:
        if hasattr(rep, 'Visibility') and rep.Visibility != 0:
            rep.Visibility = 0
        if to_remove:
            view.Representations.remove(rep)
    pv.Render(view=view)


def display_only(prs, view=None):
    """Display only the presentation in the view."""
    hide_all(view)
    if (hasattr(prs, 'Visibility') and prs.Visibility != 1):
        prs.Visibility = 1
    pv.Render(view=view)


def _check_vector_mode(vector_mode, nb_components):
    """Check vector mode."""
    if vector_mode not in ('Magnitude', 'X', 'Y', 'Z'):
        raise ValueError("Unexistent vector mode: {0}".format(vector_mode))

    if ((nb_components == 1 and (vector_mode == 'Y' or vector_mode == 'Z')) or
        (nb_components == 2 and  vector_mode == 'Z')):
        raise ValueError("Incorrect vector mode {0} for {1}-component field".
                         format(vector_mode, nb_components))


def _get_vector_component(vector_mode):
    vcomponent = -1

    if vector_mode == 'X':
        vcomponent = 0
    elif vector_mode == 'Y':
        vcomponent = 1
    elif vector_mode == 'Z':
        vcomponent = 2

    return vcomponent


def get_data_range(proxy, entity, field_name, vector_mode='Magnitude',
                   cut_off=False):
    """Get data range for the field."""
    entity_data_info = None
    field_data = proxy.GetFieldDataInformation()

    if field_name in field_data.keys():
        entity_data_info = field_data
    elif entity == EntityType.CELL:
        entity_data_info = proxy.GetCellDataInformation()
    elif entity == EntityType.NODE:
        entity_data_info = proxy.GetPointDataInformation()

    data_range = []

    if field_name in entity_data_info.keys():
        vcomp = _get_vector_component(vector_mode)
        data_range = entity_data_info[field_name].GetComponentRange(vcomp)
    else:
        pv_entity = EntityType.get_pvtype(entity)
        warnings.warn("Field {0} is unknown for {1}!".
                      format(field_name, pv_entity))

    # Cut off the range
    if cut_off and (data_range[0] <= data_range[1]):
        delta = abs(data_range[1] - data_range[0]) * GAP_COEFFICIENT
        data_range[0] += delta
        data_range[1] -= delta

    return data_range


def get_bounds(proxy):
    """Get bounds of the proxy."""
    dataInfo = proxy.GetDataInformation()
    bounds_info = dataInfo.GetBounds()
    return bounds_info


def get_x_range(proxy):
    """Get X range."""
    bounds_info = get_bounds(proxy)
    return bounds_info[0:2]


def get_y_range(proxy):
    """Get Y range."""
    bounds_info = get_bounds(proxy)
    return bounds_info[2:4]


def get_z_range(proxy):
    """Get Z range."""
    bounds_info = get_bounds(proxy)
    return bounds_info[4:6]


def is_planar_input(proxy):
    """Check if the given input is planar."""
    bounds_info = get_bounds(proxy)

    if (abs(bounds_info[0] - bounds_info[1]) <= FLT_MIN or
        abs(bounds_info[2] - bounds_info[3]) <= FLT_MIN or
        abs(bounds_info[4] - bounds_info[5]) <= FLT_MIN):
        return True

    return False


def is_data_on_cells(proxy, field_name):
    """Check if the field on cells with the given name exists."""
    cell_data_info = proxy.GetCellDataInformation()
    return (field_name in cell_data_info.keys())


def is_empty(proxy):
    """Check if the object contains any points or cells."""
    data_info = proxy.GetDataInformation()

    nb_cells = data_info.GetNumberOfCells()
    nb_points = data_info.GetNumberOfPoints()

    return not(nb_cells + nb_points)


def get_orientation(proxy):
    """Get the optimum cutting plane orientation for Plot3d."""
    orientation = Orientation.XY

    bounds = get_bounds(proxy)
    delta = [bounds[1] - bounds[0],
             bounds[3] - bounds[2],
             bounds[5] - bounds[4]]

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


def get_normal_by_orientation(orientation):
    """Get normal for the plane by its orientation."""
    normal = [0.0, 0.0, 0.0]
    if (orientation == Orientation.XY):
        normal[2] = 1.0
    elif (orientation == Orientation.ZX):
        normal[1] = 1.0
    elif (orientation == Orientation.YZ):
        normal[0] = 1.0

    return normal


def get_range_for_orientation(proxy, orientation):
    """Get source range for cutting plane orientation."""
    val_range = []
    if (orientation == Orientation.XY):
        val_range = get_z_range(proxy)
    elif (orientation == Orientation.ZX):
        val_range = get_y_range(proxy)
    elif (orientation == Orientation.YZ):
        val_range = get_x_range(proxy)

    return val_range


def get_positions(val_range, nb_planes, displacement):
    """Compute plane positions."""
    positions = []
    if (nb_planes > 1):
        distance = val_range[1] - val_range[0]
        val_range[1] = val_range[0] + (1.0 - EPS) * distance
        val_range[0] = val_range[0] + EPS * distance

        distance = val_range[1] - val_range[0]
        step = distance / (nb_planes - 1)
        abs_displacement = step * displacement
        startPos = val_range[0] - 0.5 * step + abs_displacement

        for i in xrange(nb_planes):
            pos = startPos + i * step
            positions.append(pos)
    elif (nb_planes == 1):
        pos = val_range[0] + (val_range[1] - val_range[0]) * displacement
        positions.append(pos)

    return positions


def get_contours(scalar_range, nb_contours):
    """Generate contour values."""
    contours = []
    for i in xrange(nb_contours):
        pos = scalar_range[0] + i * (
            scalar_range[1] - scalar_range[0]) / (nb_contours - 1)
        contours.append(pos)

    return contours


def get_nb_components(proxy, entity, field_name):
    """Return number of components for the field."""
    entity_data_info = None
    field_data = proxy.GetFieldDataInformation()

    if field_name in field_data.keys():
        entity_data_info = field_data
    elif entity == EntityType.CELL:
        entity_data_info = proxy.GetCellDataInformation()
    elif entity == EntityType.NODE:
        entity_data_info = proxy.GetPointDataInformation()

    nb_comp = None
    if field_name in entity_data_info.keys():
        nb_comp = entity_data_info[field_name].GetNumberOfComponents()
    else:
        pv_entity = EntityType.get_pvtype(entity)
        raise ValueError("Field {0} is unknown for {1}!".
                         format(field_name, pv_entity))

    return nb_comp


def get_scale_factor(proxy):
    """Compute scale factor."""
    if not proxy:
        return 0.0

    proxy.UpdatePipeline()
    data_info = proxy.GetDataInformation()

    nb_cells = data_info.GetNumberOfCells()
    nb_points = data_info.GetNumberOfPoints()
    nb_elements = nb_cells if nb_cells > 0  else nb_points
    bounds = get_bounds(proxy)

    volume = 1
    vol = dim = 0

    for i in xrange(0, 6, 2):
        vol = abs(bounds[i + 1] - bounds[i])
        if vol > 0:
            dim += 1
            volume *= vol

    if nb_elements == 0 or dim < 1 / VTK_LARGE_FLOAT:
        return 0

    volume /= nb_elements

    return pow(volume, 1 / dim)


def get_default_scale(prs_type, proxy, entity, field_name):
    """Get default scale factor."""
    data_range = get_data_range(proxy, entity, field_name)

    if prs_type == PrsTypeEnum.DEFORMEDSHAPE:
        EPS = 1.0 / VTK_LARGE_FLOAT
        if abs(data_range[1]) > EPS:
            scale_factor = get_scale_factor(proxy)
            return scale_factor / data_range[1]
    elif prs_type == PrsTypeEnum.PLOT3D:
        bounds = get_bounds(proxy)
        length = sqrt((bounds[1] - bounds[0]) ** 2 +
                      (bounds[3] - bounds[2]) ** 2 +
                      (bounds[5] - bounds[4]) ** 2)

        EPS = 0.3
        if data_range[1] > 0:
            return length / data_range[1] * EPS

    return 0


def get_calc_magnitude(proxy, array_entity, array_name):
    """Compute magnitude for the given vector array via Calculator.

    Return the calculator object.
    """
    calculator = None

    # Transform vector array to scalar array if possible
    nb_components = get_nb_components(proxy, array_entity, array_name)
    if (nb_components > 1):
        calculator = pv.Calculator(proxy)
        attribute_mode = "point_data"
        if array_entity != EntityType.NODE:
            attribute_mode = "cell_data"
        calculator.AttributeMode = attribute_mode
        if (nb_components == 2):
            # Workaroud: calculator unable to compute magnitude
            # if number of components equal to 2
            calculator.Function = "sqrt({0}_X^2+{0}_Y^2)".format(array_name)
        else:
            calculator.Function = "mag({0})".format(array_name)
        calculator.ResultArrayName = array_name + "_magnitude"
        calculator.UpdatePipeline()

    return calculator


def get_add_component_calc(proxy, array_entity, array_name):
    """Creates 3-component array from 2-component.

    The first two components is from the original array. The 3rd component
    is zero.
    If the number of components is not equal to 2 - return original array name.

    Return the calculator object.
    """
    calculator = None

    nb_components = get_nb_components(proxy, array_entity, array_name)
    if nb_components == 2:
        calculator = pv.Calculator(proxy)
        attribute_mode = "point_data"
        if array_entity != EntityType.NODE:
            attribute_mode = "cell_data"
        calculator.AttributeMode = attribute_mode
        expression = "iHat * {0}_X + jHat * {0}_Y + kHat * 0"
        calculator.Function = expression.format(array_name)
        calculator.ResultArrayName = array_name + "_3c"
        calculator.UpdatePipeline()

    return calculator


def select_all_cells(proxy):
    """Select all cell types.

    Used in creation of mesh/submesh presentation.
    """
    all_cell_types = proxy.CellTypes.Available
    proxy.CellTypes = all_cell_types
    proxy.UpdatePipeline()


def select_cells_with_data(proxy, on_points=None, on_cells=None):
    """Select cell types with data.

    Only cell types with data for the given fields will be selected.
    If no fields defined (neither on points nor on cells) only cell
    types with data for even one field (from available) will be selected.
    """
    all_cell_types = proxy.CellTypes.Available
    all_arrays = list(proxy.CellArrays.GetData())
    all_arrays.extend(proxy.PointArrays.GetData())

    if not all_arrays:
        file_name = proxy.FileName.split(os.sep)[-1]
        print("Warning: {0} doesn't contain any data array.".format(file_name))

    # List of cell types to be selected
    cell_types_on = []

    for cell_type in all_cell_types:
        proxy.CellTypes = [cell_type]
        proxy.UpdatePipeline()

        cell_arrays = proxy.GetCellDataInformation().keys()
        point_arrays = proxy.GetPointDataInformation().keys()

        if on_points or on_cells:
            if on_points is None:
                on_points = []
            if on_cells is None:
                on_cells = []

            if (all(array in cell_arrays for array in on_cells) and
                all(array in point_arrays for array in on_points)):
                # Add cell type to the list
                cell_types_on.append(cell_type)
        else:
            in_arrays = lambda array: ((array in cell_arrays) or
                                       (array in point_arrays))
            if any(in_arrays(array) for array in all_arrays):
                cell_types_on.append(cell_type)

    # Select cell types
    proxy.CellTypes = cell_types_on
    proxy.UpdatePipeline()


def extract_groups_for_field(proxy, field_name, field_entity, force=False):
    """Exctract only groups which have the field.

    Return ExtractGroup object, if not all groups have the field or
    the force argunent is true.
    Return the initial proxy if no groups had been filtered.
    """
    source = proxy

    # Remember the state
    initial_groups = list(proxy.Groups)

    # Get data information for the field entity
    entity_data_info = None
    field_data = proxy.GetFieldDataInformation()

    if field_name in field_data.keys():
        entity_data_info = field_data
    elif field_entity == EntityType.CELL:
        entity_data_info = proxy.GetCellDataInformation()
    elif field_entity == EntityType.NODE:
        entity_data_info = proxy.GetPointDataInformation()

    # Collect groups for extraction
    groups_to_extract = []

    for group in initial_groups:
        proxy.Groups = [group]
        proxy.UpdatePipeline()
        if field_name in entity_data_info.keys():
            groups_to_extract.append(group)

    # Restore state
    proxy.Groups = initial_groups
    proxy.UpdatePipeline()

    # Extract groups if necessary
    if force or (len(groups_to_extract) < len(initial_groups)):
        extract_group = pv.ExtractGroup(proxy)
        extract_group.Groups = groups_to_extract
        extract_group.UpdatePipeline()
        source = extract_group

    return source


def if_possible(proxy, field_name, entity, prs_type):
    """Check if the presentation is possible on the given field."""
    result = True
    if (prs_type == PrsTypeEnum.DEFORMEDSHAPE or
        prs_type == PrsTypeEnum.DEFORMEDSHAPESCALARMAP or
        prs_type == PrsTypeEnum.VECTORS or
        prs_type == PrsTypeEnum.STREAMLINES):
        nb_comp = get_nb_components(proxy, entity, field_name)
        result = (nb_comp > 1)
    elif (prs_type == PrsTypeEnum.GAUSSPOINTS):
        result = (entity == EntityType.CELL or
                  field_name in proxy.QuadraturePointArrays.Available)
    elif (prs_type == PrsTypeEnum.MESH):
        result = len(_get_group_names(proxy, field_name, entity)) > 0

    return result


def add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value):
    """Add scalar bar."""
    global _current_bar

    # Construct bar title
    title = "\n".join([field_name, str(time_value)])
    if nb_components > 1:
        title = "\n".join([title, vector_mode])

    # Create scalar bar
    scalar_bar = pv.CreateScalarBar(Enabled=1)
    scalar_bar.Orientation = 'Vertical'
    scalar_bar.Title = title
    scalar_bar.LookupTable = lookup_table

    # Set default properties same as in Post-Pro
    scalar_bar.NumberOfLabels = 5
    scalar_bar.AutomaticLabelFormat = 0
    scalar_bar.LabelFormat = '%-#6.6g'
    # Title
    scalar_bar.TitleFontFamily = 'Arial'
    scalar_bar.TitleFontSize = 8
    scalar_bar.TitleBold = 1
    scalar_bar.TitleItalic = 1
    scalar_bar.TitleShadow = 1
    # Labels
    scalar_bar.LabelFontFamily = 'Arial'
    scalar_bar.LabelFontSize = 8
    scalar_bar.LabelBold = 1
    scalar_bar.LabelItalic = 1
    scalar_bar.LabelShadow = 1

    # Add the scalar bar to the view
    pv.GetRenderView().Representations.append(scalar_bar)

    # Reassign the current bar
    _current_bar = scalar_bar

    return scalar_bar


def get_bar():
    """Get current scalar bar."""
    global _current_bar

    return _current_bar


def get_lookup_table(field_name, nb_components, vector_mode='Magnitude'):
    """Get lookup table for the given field."""
    lookup_table = pv.GetLookupTableForArray(field_name, nb_components)

    if vector_mode == 'Magnitude':
        lookup_table.VectorMode = vector_mode
    elif vector_mode == 'X':
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 0
    elif vector_mode == 'Y':
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 1
    elif vector_mode == 'Z':
        lookup_table.VectorMode = 'Component'
        lookup_table.VectorComponent = 2
    else:
        raise ValueError("Incorrect vector mode: {0}".format(vector_mode))

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
    entity_name = full_group_name.split('/')[2]

    entity = EntityType.get_type(entity_name)

    return entity


def _get_group_short_name(full_group_name):
    """Return short name of the group by its full name."""
    short_name = full_group_name.split('/')[3]

    return short_name


def _get_mesh_names(proxy):
    """Return all mesh names in the given proxy as a set."""
    groups = proxy.Groups.Available
    mesh_names = set([_get_group_mesh_name(item) for item in groups])

    return mesh_names


def _get_group_names(proxy, mesh_name, entity, wo_nogroups=False):
    """Return full names of all groups of the given entity type
    from the mesh with the given name as a list.
    """
    groups = proxy.Groups.Available

    condition = lambda item: (_get_group_mesh_name(item) == mesh_name and
                              _get_group_entity(item) == entity)
    group_names = [item for item in groups if condition(item)]

    if wo_nogroups:
        # Remove "No_Group" group
        not_no_group = lambda item: _get_group_short_name(item) != "No_Group"
        group_names = filter(not_no_group, group_names)

    return group_names


def get_time(proxy, timestamp_nb):
    """Get time value by timestamp number."""
    # Check timestamp number
    timestamps = proxy.TimestepValues.GetData()
    if ((timestamp_nb - 1) not in xrange(len(timestamps))):
        raise ValueError("Timestamp number is out of range: {0}".
                         format(timestamp_nb))

    # Return time value
    return timestamps[timestamp_nb - 1]


# Functions for building Post-Pro presentations
def ScalarMapOnField(proxy, entity, field_name, timestamp_nb,
                     vector_mode='Magnitude'):
    """Creates Scalar Map presentation on the field.

    Arguments:

    proxy -- the pipeline object, containig data
    entity -- the entity type from PrsTypeEnum
    field_name -- the field name
    timestamp_nb -- the number of time step (1, 2, ...)
    vector_mode -- the mode of transformation of vector values
    into scalar values, applicable only if the field contains vector values.
    Possible modes: 'Magnitude' - vector module;
    'X', 'Y', 'Z' - vector components.

    """
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Extract only groups with data for the field
    new_proxy = extract_groups_for_field(proxy, field_name, entity,
                                         force=True)

    # Show pipeline object
    scalarmap = pv.GetRepresentation(new_proxy)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]
    # Set properties
    scalarmap.ColorAttributeType = EntityType.get_pvtype(entity)
    scalarmap.ColorArrayName = field_name
    scalarmap.LookupTable = lookup_table

    # Add scalar bar
    bar_title = field_name + ", " + str(time_value)
    if (nb_components > 1):
        bar_title += "\n" + vector_mode
    add_scalar_bar(field_name, nb_components, vector_mode,
                   lookup_table, time_value)

    return scalarmap


def CutPlanesOnField(proxy, entity, field_name, timestamp_nb,
                     nb_planes=10, orientation=Orientation.YZ,
                     displacement=0.5, vector_mode='Magnitude'):
    """Creates cut planes on the given field."""
    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Create slice filter
    slice_filter = pv.Slice(proxy)
    slice_filter.SliceType = "Plane"

    # Set cut planes normal and get range
    normal = [0.0, 0.0, 0.0]
    val_range = []
    if (orientation == Orientation.XY):
        normal[2] = 1.0
        val_range = get_z_range(proxy)
    elif (orientation == Orientation.ZX):
        normal[1] = 1.0
        val_range = get_y_range(proxy)
    elif (orientation == Orientation.YZ):
        normal[0] = 1.0
        val_range = get_x_range(proxy)

    slice_filter.SliceType.Normal = normal

    # Set cut planes positions
    slice_filter.SliceOffsetValues = get_positions(val_range,
                                                   nb_planes,
                                                   displacement)
    cut_planes = pv.GetRepresentation(slice_filter)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    cut_planes.ColorAttributeType = EntityType.get_pvtype(entity)
    cut_planes.ColorArrayName = field_name
    cut_planes.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return cut_planes


def CutLinesOnField(proxy, entity, field_name, timestamp_nb,
                    nb_lines=10,
                    orientation1=Orientation.XY,
                    orientation2=Orientation.YZ,
                    displacement1=0.5, displacement2=0.5,
                    vector_mode='Magnitude'):
    """Creates cut lines on the given field."""
    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Create base plane
    base_plane = pv.Slice(proxy)
    base_plane.SliceType = "Plane"

    # Set base plane normal and get position
    base_normal = [0.0, 0.0, 0.0]
    valrange_base = []
    if (orientation1 == Orientation.XY):
        base_normal[2] = 1.0
        valrange_base = get_z_range(proxy)
    elif (orientation1 == Orientation.ZX):
        base_normal[1] = 1.0
        valrange_base = get_y_range(proxy)
    elif (orientation1 == Orientation.YZ):
        if (orientation2 == orientation1):
            orientation2 = Orientation.ZX
        base_normal[0] = 1.0
        valrange_base = get_x_range(proxy)

    base_plane.SliceType.Normal = base_normal

    # Set base plane position
    base_plane.SliceOffsetValues = get_positions(valrange_base, 1,
                                                 displacement1)

    # Check base plane
    base_plane.UpdatePipeline()
    if (base_plane.GetDataInformation().GetNumberOfCells() == 0):
        base_plane = proxy

    # Create cutting planes
    cut_planes = pv.Slice(base_plane)
    cut_planes.SliceType = "Plane"

    # Set cutting planes normal and get positions
    cut_normal = [0.0, 0.0, 0.0]
    valrange_cut = []
    if (orientation2 == Orientation.XY):
        cut_normal[2] = 1.0
        valrange_cut = get_z_range(proxy)
    elif (orientation2 == Orientation.ZX):
        cut_normal[1] = 1.0
        valrange_cut = get_y_range(base_plane)
    elif (orientation2 == Orientation.YZ):
        cut_normal[0] = 1.0
        valrange_cut = get_x_range(base_plane)

    cut_planes.SliceType.Normal = cut_normal

    # Set cutting planes position
    cut_planes.SliceOffsetValues = get_positions(valrange_cut, nb_lines,
                                                 displacement2)
    cut_lines = pv.GetRepresentation(cut_planes)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    cut_lines.ColorAttributeType = EntityType.get_pvtype(entity)
    cut_lines.ColorArrayName = field_name
    cut_lines.LookupTable = lookup_table

    # Set wireframe represenatation mode
    cut_lines.Representation = 'Wireframe'

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)
    return cut_lines


def VectorsOnField(proxy, entity, field_name, timestamp_nb,
                   scale_factor=None,
                   glyph_pos=GlyphPos.TAIL, glyph_type='2D Glyph',
                   is_colored=False, vector_mode='Magnitude'):
    """Creates vectors on the given field."""
    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Extract only groups with data for the field
    new_proxy = extract_groups_for_field(proxy, field_name, entity)
    source = new_proxy

    # Cell centers
    if is_data_on_cells(proxy, field_name):
        cell_centers = pv.CellCenters(source)
        cell_centers.VertexCells = 1
        source = cell_centers

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        source = calc

    # Glyph
    glyph = pv.Glyph(source)
    glyph.Vectors = vector_array
    glyph.ScaleMode = 'vector'
    glyph.MaskPoints = 0

    # Set glyph type
    glyph.GlyphType = glyph_type
    if glyph_type == '2D Glyph':
        glyph.GlyphType.GlyphType = 'Arrow'
    elif glyph_type == 'Cone':
        glyph.GlyphType.Resolution = 7
        glyph.GlyphType.Height = 2
        glyph.GlyphType.Radius = 0.2

    # Set glyph position
    if (glyph_pos == GlyphPos.TAIL):
        glyph.GlyphType.Center = [0.5, 0.0, 0.0]
    elif (glyph_pos == GlyphPos.HEAD):
        glyph.GlyphType.Center = [-0.5, 0.0, 0.0]
    elif (glyph_pos == GlyphPos.CENTER):
        glyph.GlyphType.Center = [0.0, 0.0, 0.0]

    if scale_factor is not None:
        glyph.SetScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE,
                                      new_proxy, entity, field_name)
        glyph.SetScaleFactor = def_scale

    glyph.UpdatePipeline()

    vectors = pv.GetRepresentation(glyph)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if (is_colored):
        vectors.ColorArrayName = 'GlyphVector'
    else:
        vectors.ColorArrayName = ''
    vectors.LookupTable = lookup_table

    vectors.LineWidth = 1.0

    # Set wireframe represenatation mode
    vectors.Representation = 'Wireframe'

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return vectors


def DeformedShapeOnField(proxy, entity, field_name,
                         timestamp_nb,
                         scale_factor=None, is_colored=False,
                         vector_mode='Magnitude'):
    """Creates Defromed shape on the given field."""
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Extract only groups with data for the field
    new_proxy = extract_groups_for_field(proxy, field_name, entity)

    # Do merge
    source = pv.MergeBlocks(new_proxy)

    # Cell data to point data
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pv.CellDatatoPointData()
        cell_to_point.PassCellData = 1
        source = cell_to_point

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        source = calc

    # Warp by vector
    warp_vector = pv.WarpByVector(source)
    warp_vector.Vectors = [vector_array]
    if scale_factor is not None:
        warp_vector.ScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE,
                                      proxy, entity, field_name)
        warp_vector.ScaleFactor = def_scale

    defshape = pv.GetRepresentation(warp_vector)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if is_colored:
        defshape.ColorAttributeType = EntityType.get_pvtype(entity)
        defshape.ColorArrayName = field_name
    else:
        defshape.ColorArrayName = ''
    defshape.LookupTable = lookup_table

    # Set wireframe represenatation mode
    defshape.Representation = 'Wireframe'

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return defshape


def DeformedShapeAndScalarMapOnField(proxy, entity, field_name,
                                     timestamp_nb,
                                     scale_factor=None,
                                     scalar_entity=None,
                                     scalar_field_name=None,
                                     vector_mode='Magnitude'):
    """Creates Defromed shape And Scalar Map on the given field."""
    # We don't need mesh parts with no data on them
    on_points = []
    on_cells = []

    if entity == EntityType.NODE:
        on_points.append(field_name)
    else:
        on_cells.append(field_name)

    if scalar_entity and scalar_field_name:
        if scalar_entity == EntityType.NODE:
            on_points.append(scalar_field_name)
        else:
            on_cells.append(scalar_field_name)

    select_cells_with_data(proxy, on_points, on_cells)

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Set scalar field by default
    scalar_field_entity = scalar_entity
    scalar_field = scalar_field_name
    if (scalar_field_entity is None) or (scalar_field is None):
        scalar_field_entity = entity
        scalar_field = field_name

    # Extract only groups with data for the field
    new_proxy = extract_groups_for_field(proxy, field_name, entity)

    # Do merge
    source = pv.MergeBlocks(new_proxy)

    # Cell data to point data
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pv.CellDatatoPointData(source)
        cell_to_point.PassCellData = 1
        source = cell_to_point

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        source = calc

    # Warp by vector
    warp_vector = pv.WarpByVector(source)
    warp_vector.Vectors = [vector_array]
    if scale_factor is not None:
        warp_vector.ScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE,
                                      new_proxy, entity, field_name)
        warp_vector.ScaleFactor = def_scale

    defshapemap = pv.GetRepresentation(warp_vector)

    # Get lookup table
    lookup_table = get_lookup_table(scalar_field, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, scalar_field_entity,
                                scalar_field, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    defshapemap.ColorArrayName = scalar_field
    defshapemap.LookupTable = lookup_table
    defshapemap.ColorAttributeType = EntityType.get_pvtype(scalar_field_entity)

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return defshapemap


def Plot3DOnField(proxy, entity, field_name, timestamp_nb,
                  orientation=Orientation.AUTO,
                  position=0.5, is_relative=True,
                  scale_factor=None,
                  is_contour=False, nb_contours=32,
                  vector_mode='Magnitude'):
    """Creates plot 3d on the given field."""
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Extract only groups with data for the field
    new_proxy = extract_groups_for_field(proxy, field_name, entity)

    # Do merge
    merge_blocks = pv.MergeBlocks(new_proxy)
    merge_blocks.UpdatePipeline()

    poly_data = None

    # Cutting plane

    # Define orientation if necessary (auto mode)
    plane_orientation = orientation
    if (orientation == Orientation.AUTO):
        plane_orientation = get_orientation(proxy)

    # Get cutting plane normal
    normal = get_normal_by_orientation(plane_orientation)

    if (not is_planar_input(proxy)):
        # Create slice filter
        slice_filter = pv.Slice(merge_blocks)
        slice_filter.SliceType = "Plane"

        # Set cutting plane normal
        slice_filter.SliceType.Normal = normal

        # Set cutting plane position
        val_range = get_range_for_orientation(proxy, plane_orientation)

        if (is_relative):
            slice_filter.SliceOffsetValues = get_positions(val_range, 1,
                                                           position)
        else:
            slice_filter.SliceOffsetValues = position

        slice_filter.UpdatePipeline()
        poly_data = slice_filter

    use_normal = 0
    # Geometry filter
    if not poly_data or poly_data.GetDataInformation().GetNumberOfCells() == 0:
        geometry_filter = pv.GeometryFilter(merge_blocks)
        poly_data = geometry_filter
        use_normal = 1  # TODO(MZN): temporary workaround

    warp_scalar = None
    plot3d = None
    source = poly_data

    if is_data_on_cells(poly_data, field_name):
        # Cell data to point data
        cell_to_point = pv.CellDatatoPointData(poly_data)
        cell_to_point.PassCellData = 1
        source = cell_to_point

    scalars = ['POINTS', field_name]

    # Transform vector array to scalar array if necessary
    if (nb_components > 1):
        calc = get_calc_magnitude(source, EntityType.NODE, field_name)
        scalars = ['POINTS', calc.ResultArrayName]
        source = calc

    # Warp by scalar
    warp_scalar = pv.WarpByScalar(source)
    warp_scalar.Scalars = scalars
    warp_scalar.Normal = normal
    warp_scalar.UseNormal = use_normal
    if scale_factor is not None:
        warp_scalar.ScaleFactor = scale_factor
    else:
        def_scale = get_default_scale(PrsTypeEnum.PLOT3D,
                                      proxy, entity, field_name)
        warp_scalar.ScaleFactor = def_scale

    warp_scalar.UpdatePipeline()

    if (is_contour):
        # Contours
        contour = pv.Contour(warp_scalar)
        contour.PointMergeMethod = "Uniform Binning"
        contour.ContourBy = ['POINTS', field_name]
        scalar_range = get_data_range(proxy, entity,
                                      field_name, vector_mode)
        contour.Isosurfaces = get_positions(scalar_range, nb_contours, 0)
        contour.UpdatePipeline()
        plot3d = pv.GetRepresentation(contour)
    else:
        plot3d = pv.GetRepresentation(warp_scalar)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    plot3d.ColorAttributeType = EntityType.get_pvtype(entity)
    plot3d.ColorArrayName = field_name
    plot3d.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return plot3d


def IsoSurfacesOnField(proxy, entity, field_name,
                       timestamp_nb,
                       custom_range=None, nb_surfaces=10, is_colored=True,
                       color=None, vector_mode='Magnitude'):
    """Creates Iso Surfaces on the given field."""
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Extract only groups with data for the field
    new_proxy = extract_groups_for_field(proxy, field_name, entity)

    # Do merge
    source = pv.MergeBlocks(new_proxy)

    # Transform cell data into point data if necessary
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pv.CellDatatoPointData(source)
        cell_to_point.PassCellData = 1
        source = cell_to_point

    contour_by = ['POINTS', field_name]

    # Transform vector array to scalar array if necessary
    if (nb_components > 1):
        calc = get_calc_magnitude(source, EntityType.NODE, field_name)
        contour_by = ['POINTS', calc.ResultArrayName]
        source = calc

    # Contour filter settings
    contour = pv.Contour(source)
    contour.ComputeScalars = 1
    contour.ContourBy = contour_by

    # Specify the range
    scalar_range = custom_range
    if (scalar_range is None):
        scalar_range = get_data_range(proxy, entity,
                                      field_name, cut_off=True)

    # Get contour values for the range
    surfaces = get_contours(scalar_range, nb_surfaces)

    # Set contour values
    contour.Isosurfaces = surfaces

    # Get Iso Surfaces representation
    isosurfaces = pv.GetRepresentation(contour)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set display properties
    if (is_colored):
        isosurfaces.ColorAttributeType = EntityType.get_pvtype(entity)
        isosurfaces.ColorArrayName = field_name
    else:
        isosurfaces.ColorArrayName = ''
        if color:
            isosurfaces.DiffuseColor = color
    isosurfaces.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return isosurfaces


def GaussPointsOnField(proxy, entity, field_name,
                       timestamp_nb,
                       is_deformed=True, scale_factor=None,
                       is_colored=True, color=None,
                       primitive=GaussType.SPRITE,
                       is_proportional=True,
                       max_pixel_size=256,
                       multiplier=None, vector_mode='Magnitude'):
    """Creates Gauss points on the given field.

    Arguments:

    proxy -- the pipeline object, containig data
    entity -- the entity type from PrsTypeEnum
    field_name -- the field name
    timestamp_nb -- the number of time step (1, 2, ...)
    is_deformed -- defines whether the Gauss Points will be deformed or not
    scale_factor -- the scale factor for deformation;
    will be taken into account only if is_deformed is True.
    If not passed by user, default scale will be computed.
    is_colored -- defines whether the Gauss Points will be multicolored
    color -- color of the Gauss Points. If defined, the presentation
    will be one-coloured
    primitive -- primitive type from GaussType
    is_proportional -- if True, the size of primitives will depends on
    the gauss point value
    max_pixel_size -- the maximum sizr of the Gauss Points primitive in pixels
    multiplier -- coefficient between data values and the size of primitives
    If not passed by user, default scale will be computed.
    vector_mode -- the mode of transformation of vector values into
    scalar values, applicable only if the field contains vector values.
    Possible modes: 'Magnitude' - vector module;
    'X', 'Y', 'Z' - vector components.

    """
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    proxy.UpdatePipeline(time=time_value)

    # Extract only groups with data for the field
    source = extract_groups_for_field(proxy, field_name, entity)

    # Quadrature point arrays
    qp_arrays = proxy.QuadraturePointArrays.Available

    # If no quadrature point array is passed, use cell centers
    if field_name in qp_arrays:
        generate_qp = pv.GenerateQuadraturePoints(source)
        generate_qp.SelectSourceArray = ['CELLS', 'ELGA_Offset']
        source = generate_qp
    else:
        # Cell centers
        cell_centers = pv.CellCenters(source)
        cell_centers.VertexCells = 1
        source = cell_centers

    source.UpdatePipeline()

    # Check if deformation enabled
    if is_deformed and nb_components > 1:
        vector_array = field_name
        # If the given vector array has only 2 components, add the third one
        if nb_components == 2:
            calc = get_add_component_calc(source,
                                          EntityType.NODE, field_name)
            vector_array = calc.ResultArrayName
            source = calc

        # Warp by vector
        warp_vector = pv.WarpByVector(source)
        warp_vector.Vectors = [vector_array]
        if scale_factor is not None:
            warp_vector.ScaleFactor = scale_factor
        else:
            def_scale = get_default_scale(PrsTypeEnum.DEFORMEDSHAPE, proxy,
                                          entity, field_name)
            warp_vector.ScaleFactor = def_scale
        warp_vector.UpdatePipeline()
        source = warp_vector

    # Get Gauss Points representation
    gausspnt = pv.GetRepresentation(source)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set display properties
    if is_colored:
        gausspnt.ColorAttributeType = EntityType.get_pvtype(entity)
        gausspnt.ColorArrayName = field_name
    else:
        gausspnt.ColorArrayName = ''
        if color:
            gausspnt.DiffuseColor = color

    gausspnt.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    # Set point sprite representation
    gausspnt.Representation = 'Point Sprite'

    # Point sprite settings
    gausspnt.InterpolateScalarsBeforeMapping = 0
    gausspnt.MaxPixelSize = max_pixel_size

    # Render mode
    gausspnt.RenderMode = GaussType.get_mode(primitive)

    if primitive == GaussType.SPRITE:
        # Set texture
        # TODO(MZN): replace with pvsimple high-level interface
        texture = sm.CreateProxy("textures", "SpriteTexture")
        alphamprop = texture.GetProperty("AlphaMethod")
        alphamprop.SetElement(0, 2)  # Clamp
        alphatprop = texture.GetProperty("AlphaThreshold")
        alphatprop.SetElement(0, 63)
        maxprop = texture.GetProperty("Maximum")
        maxprop.SetElement(0, 255)
        texture.UpdateVTKObjects()

        gausspnt.Texture = texture
        #gausspnt.Texture.AlphaMethod = 'Clamp'
        #gausspnt.Texture.AlphaThreshold = 63
        #gausspnt.Texture.Maximum= 255

    # Proportional radius
    gausspnt.RadiusUseScalarRange = 0
    gausspnt.RadiusIsProportional = 0

    if is_proportional:
        mult = multiplier
        if mult is None:
            mult = abs(0.1 / data_range[1])

        gausspnt.RadiusScalarRange = data_range
        gausspnt.RadiusTransferFunctionEnabled = 1
        gausspnt.RadiusMode = 'Scalar'
        gausspnt.RadiusArray = ['POINTS', field_name]
        if nb_components > 1:
            gausspnt.RadiusVectorComponent = \
                                           _get_vector_component(vector_mode)
        gausspnt.RadiusTransferFunctionMode = 'Table'
        gausspnt.RadiusScalarRange = data_range
        gausspnt.RadiusUseScalarRange = 1
        gausspnt.RadiusIsProportional = 1
        gausspnt.RadiusProportionalFactor = mult
    else:
        gausspnt.RadiusTransferFunctionEnabled = 0
        gausspnt.RadiusMode = 'Constant'
        gausspnt.RadiusArray = ['POINTS', 'Constant Radius']

    return gausspnt


def StreamLinesOnField(proxy, entity, field_name,
                       timestamp_nb,
                       direction='BOTH',
                       is_colored=False, color=None,
                       vector_mode='Magnitude'):
    """Creates Stream Lines on the given field."""
    # We don't need mesh parts with no data on them
    if entity == EntityType.NODE:
        select_cells_with_data(proxy, on_points=[field_name])
    else:
        select_cells_with_data(proxy, on_cells=[field_name])

    # Check vector mode
    nb_components = get_nb_components(proxy, entity, field_name)
    _check_vector_mode(vector_mode, nb_components)

    # Get time value
    time_value = get_time(proxy, timestamp_nb)

    # Set timestamp
    pv.GetRenderView().ViewTime = time_value
    pv.UpdatePipeline(time_value, proxy)

    # Extract only groups with data for the field
    new_proxy = extract_groups_for_field(proxy, field_name, entity)

    # Do merge
    source = pv.MergeBlocks(new_proxy)

    # Cell data to point data
    if is_data_on_cells(proxy, field_name):
        cell_to_point = pv.CellDatatoPointData(source)
        cell_to_point.PassCellData = 1
        cell_to_point.UpdatePipeline()
        source = cell_to_point

    vector_array = field_name
    # If the given vector array has only 2 components, add the third one
    if nb_components == 2:
        calc = get_add_component_calc(source, EntityType.NODE, field_name)
        vector_array = calc.ResultArrayName
        calc.UpdatePipeline()
        source = calc

    # Stream Tracer
    stream = pv.StreamTracer(source)
    stream.SeedType = "Point Source"
    stream.Vectors = ['POINTS', vector_array]
    stream.SeedType = "Point Source"
    stream.IntegrationDirection = direction
    stream.IntegratorType = 'Runge-Kutta 2'
    stream.UpdatePipeline()

    # Get representation
    if is_empty(stream):
        return None

    streamlines = pv.GetRepresentation(stream)

    # Get lookup table
    lookup_table = get_lookup_table(field_name, nb_components, vector_mode)

    # Set field range if necessary
    data_range = get_data_range(new_proxy, entity,
                                field_name, vector_mode)
    lookup_table.LockScalarRange = 1
    lookup_table.RGBPoints = [data_range[0], 0, 0, 1, data_range[1], 1, 0, 0]

    # Set properties
    if is_colored:
        streamlines.ColorAttributeType = EntityType.get_pvtype(entity)
        streamlines.ColorArrayName = field_name
    else:
        streamlines.ColorArrayName = ''
        if color:
            streamlines.DiffuseColor = color

    streamlines.LookupTable = lookup_table

    # Add scalar bar
    add_scalar_bar(field_name, nb_components,
                   vector_mode, lookup_table, time_value)

    return streamlines


def MeshOnEntity(proxy, mesh_name, entity):
    """Creates submesh of the entity type for the mesh.

    Arguments:

    proxy -- the pipeline object, containig data
    mesh_name -- the mesh name
    entity -- the entity type

    """
    # Select all cell types
    select_all_cells(proxy)

    # Get subset of groups on the given entity
    subset = _get_group_names(proxy, mesh_name, entity)

    # Select only groups of the given entity type
    proxy.Groups = subset
    proxy.UpdatePipeline()

    # Get representation object if the submesh is not empty
    prs = None
    if (proxy.GetDataInformation().GetNumberOfPoints() or
        proxy.GetDataInformation().GetNumberOfCells()):
        prs = pv.GetRepresentation(proxy)
        prs.ColorArrayName = ''

    return prs


def MeshOnGroup(proxy, group_name):
    """Creates submesh on the group.

    Arguments:

    proxy -- the pipeline object, containig data
    group_name -- the full group name

    """
    # Select all cell types
    select_all_cells(proxy)

    # Select only the group with the given name
    one_group = [group_name]
    proxy.Groups = one_group
    proxy.UpdatePipeline()

    # Get representation object if the submesh is not empty
    prs = None

    # Check if the group was set
    if proxy.Groups.GetData() == one_group:
        group_entity = _get_group_entity(group_name)
        # Check if the submesh is not empty
        nb_items = 0
        if group_entity == EntityType.NODE:
            nb_items = proxy.GetDataInformation().GetNumberOfPoints()
        elif group_entity == EntityType.CELL:
            nb_items = proxy.GetDataInformation().GetNumberOfCells()

        if nb_items:
            prs = pv.GetRepresentation(proxy)
            prs.ColorArrayName = ''

    return prs


def CreatePrsForFile(paravis_instance, file_name, prs_types,
                     picture_dir, picture_ext):
    """Build presentations of the given types for all fields of the file."""
    # Import MED file
    print("Import {0}...".format(file_name.split(os.sep)[-1]), end='')

    try:
        paravis_instance.ImportFile(file_name)
        proxy = pv.GetActiveSource()
        if proxy is None:
            print ("FAILED")
        else:
            proxy.UpdatePipeline()
            print("OK")
    except:
        print ("FAILED")
    else:
        # Get view
        view = pv.GetRenderView()

        # Create required presentations for the proxy
        CreatePrsForProxy(proxy, view, prs_types,
                          picture_dir, picture_ext)


def _create_prs(prs_type, proxy, field_entity, field_name, timestamp_nb):
    """Internal method.
    Build presentation of the given type on the given field and
    timestamp number.
    Set the presentation properties like visu.CreatePrsForResult() do.
    """
    prs = None

    if prs_type == PrsTypeEnum.SCALARMAP:
        prs = ScalarMapOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.CUTPLANES:
        prs = CutPlanesOnField(proxy, field_entity, field_name, timestamp_nb,
                               orientation=Orientation.ZX)
    elif prs_type == PrsTypeEnum.CUTLINES:
        prs = CutLinesOnField(proxy, field_entity, field_name, timestamp_nb,
                              orientation1=Orientation.XY,
                              orientation2=Orientation.ZX)
    elif prs_type == PrsTypeEnum.DEFORMEDSHAPE:
        prs = DeformedShapeOnField(proxy, field_entity,
                                   field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.DEFORMEDSHAPESCALARMAP:
        prs = DeformedShapeAndScalarMapOnField(proxy, field_entity,
                                               field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.VECTORS:
        prs = VectorsOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.PLOT3D:
        prs = Plot3DOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.ISOSURFACES:
        prs = IsoSurfacesOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.GAUSSPOINTS:
        prs = GaussPointsOnField(proxy, field_entity, field_name, timestamp_nb)
    elif prs_type == PrsTypeEnum.STREAMLINES:
        prs = StreamLinesOnField(proxy, field_entity, field_name, timestamp_nb)
    else:
        raise ValueError("Unexistent presentation type.")

    return prs


def CreatePrsForProxy(proxy, view, prs_types, picture_dir, picture_ext):
    """Build presentations of the given types for all fields of the proxy.
    Save snapshots in graphics files (type depends on the given extension).
    Stores the files in the given directory.

    Arguments:

    proxy -- the pipeline object, containig data arrays
    view  -- the render view
    prs_types  -- the list of presentation types to build
    picture_dir   -- the directory path for saving snapshots
    picture_ext   -- graphics files extension (determines file type)

    """
    # List of the field names
    field_names = list(proxy.PointArrays.GetData())
    nb_on_nodes = len(field_names)
    field_names.extend(proxy.CellArrays.GetData())

    # Add path separator to the end of picture path if necessery
    if not picture_dir.endswith(os.sep):
        picture_dir += os.sep

    # Mesh Presentation
    if PrsTypeEnum.MESH in prs_types:
        # Create Mesh presentation. Build all possible submeshes.

        # Remember the current state
        groups = list(proxy.Groups)

        # Iterate on meshes
        mesh_names = _get_mesh_names(proxy)
        for mesh_name in mesh_names:
            # Build mesh on nodes and cells
            for entity in (EntityType.NODE, EntityType.CELL):
                entity_name = EntityType.get_name(entity)
                if if_possible(proxy, mesh_name, entity, PrsTypeEnum.MESH):
                    print("Creating submesh on {0} for '{1}' mesh... ".
                          format(entity_name, mesh_name), end='')
                    prs = MeshOnEntity(proxy, mesh_name, entity)
                    if prs is None:
                        print("FAILED")
                        continue
                    else:
                        print("OK")
                    # Construct image file name
                    pic_name = "{folder}{mesh}_{type}.{ext}".format(
                        folder=picture_dir,
                        mesh=mesh_name,
                        type=entity_name,
                        ext=picture_ext)

                    # Show and dump the presentation into a graphics file
                    process_prs_for_test(prs, view, pic_name, False)

                # Build submesh on all groups of the mesh
                mesh_groups = _get_group_names(proxy, mesh_name,
                                               entity, wo_nogroups=True)
                for group in mesh_groups:
                    print("Creating submesh on group {0}... ".
                          format(group), end='')
                    prs = MeshOnGroup(proxy, group)
                    if prs is None:
                        print("FAILED")
                        continue
                    else:
                        print("OK")
                    # Construct image file name
                    pic_name = "{folder}{group}.{ext}".format(
                        folder=picture_dir,
                        group=group.replace('/', '_'),
                        ext=picture_ext)

                    # Show and dump the presentation into a graphics file
                    process_prs_for_test(prs, view, pic_name, False)

        # Restore the state
        proxy.Groups = groups
        proxy.UpdatePipeline()

    # Presentations on fields
    for (i, field_name) in enumerate(field_names):
        # Select only the current field:
        # necessary for getting the right timestamps
        cell_arrays = proxy.CellArrays.GetData()
        point_arrays = proxy.PointArrays.GetData()
        field_entity = None
        if (i >= nb_on_nodes):
            field_entity = EntityType.CELL
            proxy.PointArrays.DeselectAll()
            proxy.CellArrays = [field_name]
        else:
            field_entity = EntityType.NODE
            proxy.CellArrays.DeselectAll()
            proxy.PointArrays = [field_name]

        # Get timestamps
        proxy.UpdatePipelineInformation()
        timestamps = proxy.TimestepValues.GetData()

        # Restore fields selection state
        proxy.CellArrays = cell_arrays
        proxy.PointArrays = point_arrays
        proxy.UpdatePipelineInformation()

        for prs_type in prs_types:
            # Ignore mesh presentation
            if prs_type == PrsTypeEnum.MESH:
                continue

            # Get name of presentation type
            prs_name = PrsTypeEnum.get_name(prs_type)

            # Build the presentation if possible
            possible = if_possible(proxy, field_name,
                                   field_entity, prs_type)
            if possible:
                # Presentation type for graphics file name
                f_prs_type = prs_name.replace(' ', '').upper()

                for timestamp_nb in xrange(1, len(timestamps) + 1):
                    time = timestamps[timestamp_nb - 1]
                    print("Creating {0} on {1}, time = {2}... ".
                          format(prs_name, field_name, time), end='')
                    prs = _create_prs(prs_type, proxy,
                                      field_entity, field_name, timestamp_nb)
                    if prs is None:
                        print("FAILED")
                        continue
                    else:
                        print("OK")

                    # Construct image file name
                    pic_name = "{folder}{field}_{time}_{type}.{ext}".format(
                        folder=picture_dir,
                        field=field_name,
                        time=time,
                        type=f_prs_type,
                        ext=picture_ext)

                    # Show and dump the presentation into a graphics file
                    process_prs_for_test(prs, view, pic_name)
