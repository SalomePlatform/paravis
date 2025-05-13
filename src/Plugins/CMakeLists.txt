# Copyright (C) 2010-2025  CEA, EDF
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

SET(_subdirs
  MEDReader
  MEDWriter
#  TableReader
  SimpleMode
  ElevationSurface
  ScaleVector
  EllipseBuilder
  ArrayRenamer
  JSONReader
  DevelopedSurface
  StaticMesh
  GaussToCell
  VoroGauss
  CADRepresentation
  )

IF(NOT SALOME_LIGHT_ONLY)
  IF(SALOME_PARAVIS_BUILD_CORBA_PLUGINS)
    LIST(APPEND _subdirs ParaMEDCorba)
  ENDIF()
ENDIF()

IF(SALOME_PARAVIS_USE_GEOM_SOURCE)
  LIST(APPEND _subdirs ParaGEOMCorba)
ENDIF()

IF(SALOME_PARAVIS_USE_SMESH_SOURCE)
  LIST(APPEND _subdirs ParaSMESHCorba)
ENDIF()

if(SALOME_USE_MPI)
  # see https://stackoverflow.com/questions/20239334/cmake-set-subdirectory-options
  # force to ON MEDREADER option instead of a simple set(MEDREADER_USE_MPI ON)
  set(MEDREADER_USE_MPI ON CACHE BOOL "Activate parallel reading for MEDREADER" FORCE)
endif(SALOME_USE_MPI)

FOREACH(_dir ${_subdirs})
  ADD_SUBDIRECTORY(${_dir})
ENDFOREACH()
