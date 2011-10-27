# Copyright (C) 2010-2011  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
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

FIND_PACKAGE(ParaView REQUIRED)

SET(PARAVIEW_INCLUDES -I${PARAVIEW_INCLUDE_DIRS})

FIND_LIBRARY(QtPython QtPython ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(QVTK QVTK ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkClientServer vtkClientServer ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkGenericFiltering vtkGenericFiltering ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkInfovis vtkInfovis ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkVolumeRendering vtkVolumeRendering ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkPVCommandOptions vtkPVCommandOptions ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkPVFilters vtkPVFilters ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkPVServerCommon vtkPVServerCommon ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(vtkPVServerManager vtkPVServerManager ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(pqApplicationComponents pqApplicationComponents ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(pqComponents pqComponents ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(pqCore pqCore ${PARAVIEW_LIBRARY_DIRS})
FIND_LIBRARY(pqWidgets pqWidgets ${PARAVIEW_LIBRARY_DIRS})

SET(PARAVIEW_LIBS
  ${QtPython}
  ${QVTK}
  ${vtkClientServer}
  ${vtkGenericFiltering}
  ${vtkInfovis}
  ${vtkVolumeRendering}
  ${vtkPVCommandOptions}
  ${vtkPVFilters}
  ${vtkPVServerCommon}
  ${vtkPVServerManager}
  ${pqApplicationComponents}
  ${pqComponents}
  ${pqCore}
  ${pqWidgets}
  )
