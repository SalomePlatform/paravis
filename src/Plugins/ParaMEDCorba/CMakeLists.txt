# Copyright (C) 2020-2024  CEA, EDF
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
project(ParaMEDCorba)

include(GNUInstallDirs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

find_package(SalomeFIELDS)

include_directories( ${CMAKE_CURRENT_SOURCE_DIR}/plugin/ParaMEDCorbaModule )

set("_paraview_plugin_default_${CMAKE_PROJECT_NAME}" ON)
paraview_plugin_scan(
  ENABLE_BY_DEFAULT YES
  PLUGIN_FILES      "${CMAKE_CURRENT_SOURCE_DIR}/plugin/paraview.plugin"
  PROVIDES_PLUGINS  plugins
  REQUIRES_MODULES  required_modules)

foreach(module IN LISTS required_modules)
  if(NOT TARGET "${module}")
    message("Missing required module: ${module}")
    return()
  endif()
endforeach()

set(BUILD_SHARED_LIBS ON)
paraview_plugin_build(
  RUNTIME_DESTINATION "${CMAKE_INSTALL_BINDIR}"
  LIBRARY_DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  LIBRARY_SUBDIRECTORY "${PARAVIEW_PLUGIN_SUBDIR}"
  PLUGINS ${plugins}
  AUTOLOAD ${plugins})


########################

  # CMAKE_MINIMUM_REQUIRED( VERSION 2.8.7 )
  # IF(WIN32)
  #   CMAKE_POLICY(SET CMP0020 OLD) # disable automatic linking to qtmain.lib 
  # ENDIF(WIN32)

  # ## !!! This definition corrupts wrapping process
  # #SET( LIBRARY_OUTPUT_PATH ${ParaMEDCorba_BINARY_DIR}/bin CACHE INTERNAL "Single place for output" )
  # #SET( EXECUTABLE_OUTPUT_PATH ${ParaMEDCorba_BINARY_DIR}/bin  CACHE INTERNAL "Single place for output" )
  # MARK_AS_ADVANCED(
  #   LIBRARY_OUTPUT_PATH
  #   EXECUTABLE_OUTPUT_PATH
  # )

  # FIND_PACKAGE( ParaView COMPONENTS pqCore NO_MODULE)

  # IF( ParaView_FOUND )
    
  #   INCLUDE( ${PARAVIEW_USE_FILE} )
    
  #   FIND_PACKAGE( VTK COMPONENTS vtkClientServer NO_MODULE NO_DEFAULT_PATH)
  #   INCLUDE( ${VTK_USE_FILE} )

  #   SET( PARACORBAPLUG_SRCS
  #     ${ParaMEDCorba_SOURCE_DIR}/vtkParaMEDCorbaSource.cxx
  #     )

  #   ADD_LIBRARY (ParaMEDMEM2VTK SHARED VTKMEDCouplingMeshClient.cxx VTKMEDCouplingUMeshClient.cxx VTKMEDCouplingCMeshClient.cxx
  #     VTKMEDCouplingFieldClient.cxx VTKParaMEDFieldClient.cxx VTKMEDCouplingMultiFieldsClient.cxx VTKMEDCouplingCurveLinearMeshClient.cxx)
    
  #   ADD_PARAVIEW_PLUGIN( ParaMEDCorbaPlugin "1.0"
  #     SERVER_MANAGER_XML ParaMEDCorbaServerManager.xml
  #     SERVER_MANAGER_SOURCES ${PARACORBAPLUG_SRCS} )
    
  #   INCLUDE_DIRECTORIES(${OMNIORB_INCLUDE_DIR} ${FIELDS_ROOT_DIR}/include/salome ${KERNEL_ROOT_DIR}/include/salome)
  #   ADD_DEFINITIONS(${OMNIORB_DEFINITIONS})

  #   TARGET_LINK_LIBRARIES(ParaMEDMEM2VTK
  #     #vtkCommon
  #     #vtkFiltering
  #     #vtkGraphics
  #     vtkClientServer
  #     vtkPVVTKExtensionsRendering
  #     pqCore
  #     ${OMNIORB_LIBRARIES}
  #     ${FIELDS_SalomeIDLMED}
  #     ${KERNEL_SalomeIDLKernel}
  #     )
  #   TARGET_LINK_LIBRARIES(ParaMEDCorbaPlugin
  #     ParaMEDMEM2VTK
  #     )
    
  #   #Build library with a static runtime and ignore MSVCRT.lib on WINDOWS
  #   IF(WIN32)
  #     IF(CMAKE_BUILD_TYPE STREQUAL Debug)
  #       SET_TARGET_PROPERTIES(ParaMEDMEM2VTK PROPERTIES LINK_FLAGS "/NODEFAULTLIB:MSVCRTD")
  #       SET_TARGET_PROPERTIES(ParaMEDMEM2VTK PROPERTIES COMPILE_FLAGS "/MTd")
  #     ELSE()
  #       SET_TARGET_PROPERTIES(ParaMEDMEM2VTK PROPERTIES LINK_FLAGS "/NODEFAULTLIB:MSVCRT")
  #       SET_TARGET_PROPERTIES(ParaMEDMEM2VTK PROPERTIES COMPILE_FLAGS "/MT")
  #     ENDIF()
  #   ENDIF(WIN32)

  #   INSTALL(TARGETS ParaMEDCorbaPlugin DESTINATION lib/paraview)

  #   INSTALL(TARGETS ParaMEDMEM2VTK DESTINATION lib/salome)
    
  #   ADD_EXECUTABLE( testParaMEDCorba
  #     ${ParaMEDCorba_SOURCE_DIR}/testParaMEDCorba.cxx
  #     )
  #   # SET_TARGET_PROPERTIES(testCorbaPlugin PROPERTIES COMPILE_FLAGS "${OMNIORB_DEFINITIONS}")

  #   # RNV: ${vtkgl2ps_LIBRARIES} was added for Fedora 26 OS    
  #   TARGET_LINK_LIBRARIES( testParaMEDCorba
  #     ParaMEDCorbaPlugin
  #     ${vtkgl2ps_LIBRARIES}
  #     )
    
  #   INSTALL(FILES ParaMEDCorbaServerManager.xml DESTINATION share)
    
  
  # ENDIF( ParaView_FOUND )

