# Copyright (C) 2010-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

"""
This module provides auxiliary classes, functions and variables for testing.
"""

#from __future__ import print_function

from math import fabs
import os
import tempfile
import getpass
from datetime import date
import struct
import sys

# Auxiliary variables

# Data directory
samples_dir = os.getenv("DATA_DIR")
datadir = None
tablesdir = None
texturesdir = None
if samples_dir is not None:
    samples_dir = os.path.normpath(samples_dir)
    datadir = samples_dir + "/MedFiles/"
    tablesdir = samples_dir + "/Tables/"
    texturesdir = samples_dir + "/Textures/"

# Graphics files extension
pictureext = os.getenv("PIC_EXT")
if pictureext == None:
    pictureext = "png"


# Auxiliary classes
class RepresentationType:
    """
    Types of representation.
    """
    OUTLINE = 0
    POINTS = 1
    WIREFRAME = 2
    SURFACE = 3
    SURFACEEDGES = 4
    VOLUME = 5
    POINTSPRITE = 6

    _type2name = {OUTLINE: 'Outline',
                  POINTS: 'Points',
                  WIREFRAME: 'Wireframe',
                  SURFACE: 'Surface',
                  SURFACEEDGES: 'Surface With Edges',
                  VOLUME: 'Volume',
                  POINTSPRITE: 'Point Sprite'}

    @classmethod
    def get_name(cls, type):
        """Return paraview representation type by the primitive type."""
        return cls._type2name[type]


# Auxiliary functions
def test_values(value, et_value, check_error=0):
    """Test values."""
    error = 0
    length = len(value)
    et_length = len(et_value)
    if (length != et_length):
        err_msg = "ERROR!!! There is different number of created " + str(length) + " and etalon " + str(et_length) + " values!!!"
        print err_msg
        error = error + 1
    else:
        for i in range(et_length):
            if abs(et_value[i]) > 1:
                max_val = abs(0.001 * et_value[i])
                if abs(et_value[i] - value[i]) > max_val:
                    err_msg = "ERROR!!! Got value " + str(value[i]) + " is not equal to etalon value " + str(ret_value[i]) + "!!!"
                    print err_msg
                    error = error + 1
            else:
                max_val = 0.001
                if abs(et_value[i] - value[i]) > max_val:
                    err_msg = "ERROR!!! Got value " + value[i] + " is not equal to etalon value " + et_value[i] + "!!!"
                    error = error + 1
    if check_error and error > 0:
        err_msg = ("There is(are) some error(s) was(were) found... "
                   "For more info see ERRORs above...")
        raise RuntimeError(err_msg)
    return error


def get_picture_dir(subdir):
    res_dir = os.getenv("PARAVIS_TEST_PICS")
    if not res_dir:
        # Add username and current date to unify the directory
        cur_user = getpass.getuser()
        cur_date = date.today().strftime("%Y%m%d")
        res_dir = tempfile.gettempdir() + \
            "/pic_" + cur_user + \
            "/test_" + cur_date
    # Add subdirectory for the case to the directory path
    res_dir += "/" + subdir
    # Create the directory if doesn't exist
    res_dir = os.path.normpath(res_dir)
    if not os.path.exists(res_dir):
        os.makedirs(res_dir)
    else:
        # Clean the directory
        for root, dirs, files in os.walk(res_dir):
            for f in files:
                os.remove(os.path.join(root, f))

    return res_dir


def call_and_check(prs, property_name, value, do_raise=1, compare_toler=-1.0):
    """Utility function for 3D viewer test for common check of different
    types of presentation parameters set"""
    if property_name == 'Representation':
        if value in prs.GetProperty('RepresentationTypesInfo'):
            prs.SetPropertyWithName(property_name, value)
            error_string = None
        else:
            error_string = (str(value) + " value of " + property_name + " is not available for this type of presentations")
    else:
        try:
            prs.SetPropertyWithName(property_name, value)
        except ValueError:
            error_string = (str(value) + "value of " + property_name + " is not available for this type of presentations")
        else:
            error_string = None
    is_good = (error_string is None)
    if not is_good:
        if do_raise:
            raise RuntimeError(error_string)
        else:
            print error_string
    else:
        # compare just set value and the one got from presentation
        really_set_value = prs.GetPropertyValue(property_name)
        is_equal = 1
        if compare_toler > 0:
            is_equal = (fabs(really_set_value - value) < compare_toler)
        else:
            is_equal = (really_set_value == value)
        if not is_equal:
            msg = str(really_set_value) + " has been set instead"
            if do_raise:
                raise RuntimeError(msg)
            else:
                print msg
                is_good = False

    return is_good


def compare_lists(value, et_value, check_error=0, eps=1e-04):
    """
    Compare two lists: the same length and equality of corresponding items
    param value - list to be compared
    param et_value - etalon list
    param check_error - flag to catch exception if errors>0
    check_error=0 no exception, check_error !=0 catch exception
    param eps - defines tolerance for comparison
    return error - number of errors
    """

    error=0
    length = len(value)
    et_length = len(et_value)
    if length != et_length:
        print "ERROR!!! There is different number of items in created ", length, " and etalon ", et_length, " lists!!!"
        error=error+1
    else:
        for i in range(et_length):
            if abs(et_value[i]) > 1:
                MAX = abs(eps*et_value[i])
            else:
                MAX = eps
            if abs(et_value[i] - value[i])> MAX:
                print "ERROR!!!", i, "-th  item", value[i], " is not equal to etalon item", et_value[i], "!!!"
                error=error+1
    if check_error and error > 0:
        raise RuntimeError, "There is(are) some error(s) was(were) found... For more info see ERRORs above..."
    return error


def setShaded(view, shading):
    """Utility function to set shaded mode in view"""
    if shading == 0:
        view.LightDiffuseColor = [1, 1, 1]
    if shading == 1:
        view.LightDiffuseColor = [0, 0, 0]


def TimeStampId(proxy):
    """Return tuple for the given MED proxy: (mesh_name, {field_name: [entity, timestamp_id]})
    Originally defined in KERNEL_TEST/Tools/CommonFunctions file.
    """
    import presentations
    mesh_name = presentations.get_mesh_full_names(proxy).pop()
    iterations = {}

    # get list of field names
    all_fields = proxy.GetProperty("FieldsTreeInfo")[::2]

    # get timestamps
    timestamps = proxy.TimestepValues.GetData()
    timestamp_nb = len(timestamps)

    for field in all_fields:
        entity = presentations.get_field_entity(field)
        field_short_name = presentations.get_field_short_name(field)

        iterations[field_short_name] = [entity, timestamp_nb]

    return mesh_name, iterations


def Import_Med_Field(filename, field_names, check_errors=0, prs=[]):
    """Builds presentations on the given fields of the MED file.
    Originally defined in VISU_TEST/Grids/visu/ImportMedField/begin file.

    Arguments:
      filename     : the full path to med file
      field_names  : the list of field names (for ex: ["pression","temperature","vitesse"])
      prs          : [[0,1,...], [], []]; empty list (sublist(s)) is ignored
                     0-VISU.TGAUSSPOINTS
                     1-VISU.TSCALARMAP
                     2-VISU.TISOSURFACE
                     3-VISU.TCUTPLANES
                     4-VISU.TCUTLINES
                     5-VISU.TDEFORMEDSHAPE
                     6-VISU.TVECTORS
                     7-VISU.TSTREAMLINES
                     8-VISU.TPLOT3D
                     9-VISU.TSCALARMAPONDEFORMEDSHAPE
    """
    import presentations

    nb_errors = 0

    print "File: ", filename

    # check the file accessibility
    if not os.access(filename, os.F_OK):
        msg = "File " + filename + " does not exist!!!"
        raise RuntimeError, msg

    # import MED file
    import pvsimple
    pvsimple.OpenDataFile(filename)
    proxy = presentations.pvs.GetActiveSource()
    if proxy is None:
        raise RuntimeError, "ERROR!!! Can't import file!!!"

    for i in range(len(field_names)):
        print "Name of the field: ", field_names[i]

        if len(prs) != 0:
            if len(prs[i]) != 0:
                mesh_name, iterations = TimeStampId(proxy)

                if iterations.has_key(field_names[i]):
                    entity = iterations[field_names[i]][0]
                    iteration = iterations[field_names[i]][1]
                else:
                    msg="There is no information about TimeStampId of the " + field_names[i] + " field!!!"
                    raise RuntimeError, msg

                err = nb_errors

                for type in prs[i]:
                    try:
                        if type==0:
                            if presentations.GaussPointsOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created GaussPoints presentation is None!!!"; nb_errors+=1
                        if type==1:
                            if presentations.ScalarMapOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created ScalarMap presentation is None!!!"; nb_errors+=1
                        if type==2:
                            if presentations.IsoSurfacesOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created IsoSurfaces presentation is None!!!"; nb_errors+=1
                        if type==3:
                            if presentations.CutPlanesOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created CutPlanes presentation is None!!!"; nb_errors+=1
                        if type==4:
                            if presentations.CutLinesOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created CutLines presentation is None!!!"; nb_errors+=1
                        if type==5:
                            if presentations.DeformedShapeOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created DeformedShape presentation is None!!!"; nb_errors+=1
                        if type==6:
                            if presentations.VectorsOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created Vectors presentation is None!!!"; nb_errors+=1
                        if type==7:
                            if presentations.StreamLinesOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created StreamLines presentation is None!!!"; nb_errors+=1
                        if type==8:
                            if presentations.Plot3DOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created Plot3D presentation is None!!!"; nb_errors+=1
                        if type==9:
                            if presentations.DeformedShapeAndScalarMapOnField(proxy, entity, field_names[i], iteration) is None:
                                print "ERROR!!! Created ScalarMapOnDeformedShape presentation is None!!!"; nb_errors+=1
                    except ValueError:
                        """ This exception comes from get_nb_components(...) function.
                            The reason of exception is an implementation of MEDReader
                            activating the first leaf when reading MED file (refer to
                            MEDFileFieldRepresentationTree::activateTheFirst() and
                            MEDFileFieldRepresentationTree::getTheSingleActivated(...) methods).
                        """
                        print "ValueError exception is catched"
                        continue

                # check if number of errors has increased
                if err == nb_errors:
                    print "Presentation(s) creation...OK"

    if nb_errors > 0 and check_errors:
        raise RuntimeError, "Errors occured!!! For more information see ERRORs above..."
    else:
        return nb_errors

def delete_with_inputs(obj):
    """Deletes the given object with all its inputs"""
    import pvsimple
    import presentations

    obj_to_delete = obj
    while obj_to_delete is not None:
        tmp_obj = obj_to_delete
        obj_to_delete = None
        if hasattr(tmp_obj, 'Input'):
            obj_to_delete = tmp_obj.Input

        presentations.delete_pv_object(tmp_obj)

def get_png_picture_resolution(infile):
    """Returns size (width, height) of the PNG image"""
    f = open(infile, 'rb')
    data = f.read(24)
    f.close()
    if not (data[:8] == '\211PNG\r\n\032\n'and (data[12:16] == 'IHDR')):
        raise RuntimeError("File '%s' is not PNG image"%(infile))

    w, h = struct.unpack('>LL', data[16:24])
    width = int(w)
    height = int(h)
    return (width,height)

def save_trace(afile,atrace):
    """Saves atrace in afile"""
    f = open(afile, 'w')
    f.write(atrace)
    f.close()

def compare_view_to_ref_image(view, image_file, threshold=10):
  import vtk.test.Testing
  # warning: vtkGetTempDir looks at sys.argv contents...
  save_sys_argv = sys.argv
  sys.argv = []
  vtk.test.Testing.VTK_TEMP_DIR = vtk.util.misc.vtkGetTempDir()

  try:
    vtk.test.Testing.compareImage(view.GetRenderWindow(),
                                  image_file,
                                  threshold=threshold)
    vtk.test.Testing.interact()
  except:
    sys.argv = save_sys_argv
    print "<b>ERROR!!! Pictures differs from reference image !!!</b>";
    print "Picture: "+image_file
    raise
    pass
  sys.argv = save_sys_argv
