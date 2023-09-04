# Copyright (C) 2010-2023  CEA, EDF
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

cmake_minimum_required(VERSION 3.8)
project(MEDReader)

option(BUILD_SHARED_LIBS "Build shared libraries" ON)
option(MEDREADER_USE_MPI "Activate parallel reading for MEDREADER" OFF)

find_package(ParaView REQUIRED)
if(MEDREADER_USE_MPI)
  add_definitions(-DMEDREADER_USE_MPI)
endif(MEDREADER_USE_MPI)

enable_testing()

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

set("_paraview_plugin_default_${CMAKE_PROJECT_NAME}" ON)
paraview_plugin_scan(
  ENABLE_BY_DEFAULT YES
  PLUGIN_FILES      "${CMAKE_CURRENT_SOURCE_DIR}/plugin/paraview.plugin"
  PROVIDES_PLUGINS  plugins
  REQUIRES_MODULES  required_modules)

foreach (module IN LISTS required_modules)
  if (NOT TARGET "${module}")
    message("Missing required module: ${module}")
    return ()
  endif ()
endforeach ()

paraview_plugin_build(
  RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY_SUBDIRECTORY "${PARAVIEW_PLUGIN_SUBDIR}"
  PLUGINS ${plugins})

if(TARGET VTK::Python)

  vtk_module_python_default_destination(python_destination)

  vtk_module_wrap_python(
    MODULES         MEDReaderIO
    WRAPPED_MODULES medr_wrapped_modules
    TARGET          MEDREADER::Python
    INSTALL_EXPORT  MEDReaderVTKPython
    PYTHON_PACKAGE  "MEDReaderVTK"
    MODULE_DESTINATION  "${python_destination}"
    CMAKE_DESTINATION   "${CMAKE_INSTALL_LIBDIR}/cmake/MEDReaderVTKPython"
    LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
    INSTALL_HEADERS ON
    BUILD_STATIC    OFF)

  # Generate __init__.py
  set(python_modules)
  foreach(module ${medr_wrapped_modules})
    list(APPEND python_modules "'${module}'")
  endforeach()

  set(InitContent "__all__ = ['vtkMEDReader']")

  file(GENERATE
    OUTPUT  "${CMAKE_BINARY_DIR}/${python_destination}/MEDReaderVTK/__init__.py"
    CONTENT "${InitContent}")
  install(
    FILES       "${CMAKE_BINARY_DIR}/${python_destination}/MEDReaderVTK/__init__.py"
    DESTINATION "${python_destination}/MEDReaderVTK/")

  # Install MEDReaderVTKPython
  export(
    EXPORT    MEDReaderVTKPython
    NAMESPACE MEDREADER::
    FILE "${CMAKE_BINARY_DIR}/${python_destination}/MEDReaderVTKPython/MEDReaderVTKPython-targets.cmake")
  install(
    EXPORT    MEDReaderVTKPython
    NAMESPACE MEDREADER::
    FILE      MEDReaderVTKPython-targets.cmake
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/MEDReaderVTKPython")

endif()
