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

project(vtkEDFOverloads)

cmake_minimum_required(VERSION 2.8)

option(GENERATE_PARAVIEW_PLUGIN "warp all classes for usage in paraview" ON)

if(${GENERATE_PARAVIEW_PLUGIN})
  find_package(ParaView REQUIRED)
  include(${PARAVIEW_USE_FILE})
else(${GENERATE_PARAVIEW_PLUGIN})
  find_package(VTK REQUIRED)
  include(${VTK_USE_FILE})
endif(${GENERATE_PARAVIEW_PLUGIN})

set(vtkEDFOverloads_CLASSES  
  vtkEDFCutter
  vtkEDFFactory
)

set(vtkEDFOverloads_CXX)
set(vtkEDFOverloads_H)

foreach(class ${vtkEDFOverloads_CLASSES})
  set(vtkEDFOverloads_CXX ${vtkEDFOverloads_CXX} ${class}.cxx)
  set(vtkEDFOverloads_H ${vtkEDFOverloads_H} ${class}.h)
endforeach(class ${vtkEDFOverloads_CLASSES})

if(${GENERATE_PARAVIEW_PLUGIN})
  add_paraview_plugin(vtkEDFOverloads "0.0"
        SERVER_MANAGER_SOURCES ${vtkEDFOverloads_CXX})
else(${GENERATE_PARAVIEW_PLUGIN})
  add_library(vtkEDFOverloads SHARED ${vtkEDFOverloads_CXX})
  link_libraries(vtkEDFOverloads ${VTK_LIBRARIES})
endif(${GENERATE_PARAVIEW_PLUGIN})

INSTALL(
    TARGETS vtkEDFOverloads
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib/paraview
    ARCHIVE DESTINATION .
)
