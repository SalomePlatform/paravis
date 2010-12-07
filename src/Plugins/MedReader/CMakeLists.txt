project(MedReader)
	 
cmake_minimum_required(VERSION 2.4)
	
if(COMMAND cmake_policy)
	 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)
	
	# Setup output directories.
set (LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib CACHE INTERNAL
	  "Single output directory for building all libraries.")
set (EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin CACHE INTERNAL
	  "Single output directory for building all executables.")
	
SET(MED_READER_MAJOR_VERSION 3)
SET(MED_READER_MINOR_VERSION 0)
SET(MED_READER_BUILD_VERSION 1)
SET(MED_READER_VERSION
    "${MED_READER_MAJOR_VERSION}.${MED_READER_MINOR_VERSION}.${MED_READER_BUILD_VERSION}")

#
# Load the ParaView/VTK packages
#
set(MedReader_BUILD_ParaView_Plugin ON CACHE BOOL "Build the ParaView plugin")

if(MedReader_BUILD_ParaView_Plugin)
    FIND_PACKAGE(ParaView REQUIRED)
    if(NOT ParaView_FOUND)
        MESSAGE(FATAL_ERROR "Please locate ParaView." )
    ENDIF(NOT ParaView_FOUND)
    INCLUDE(${PARAVIEW_USE_FILE})
else(MedReader_BUILD_ParaView_Plugin)
    FIND_PACKAGE(VTK REQUIRED)
    if(NOT VTK_FOUND)
        MESSAGE(FATAL_ERROR "Please locate VTK." )
    ENDIF(NOT VTK_FOUND)
    INCLUDE(${VTK_USE_FILE})
endif(MedReader_BUILD_ParaView_Plugin)

	 
# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS})
#
# Find dependant Library
#
SET(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake ${CMAKE_MODULE_PATH} )
FIND_PACKAGE(MED REQUIRED)
FIND_PACKAGE(HDF5 REQUIRED)
include_directories(${MED_INCLUDE_DIR})
include_directories(${HDF5_INCLUDE_DIR})

#
# Build the vtkMedReader
#
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/IO ${CMAKE_CURRENT_SOURCE_DIR}/Filtering)
add_subdirectory(IO)
add_subdirectory(Filtering)

#
# Build the parallel vtkPMedReader
#
set(MedReader_BUILD_PARALLEL ${VTK_USE_PARALLEL} CACHE BOOL "Build the Med Reader with parallel support.")

if(MedReader_BUILD_PARALLEL)
  include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Parallel)
  add_subdirectory(Parallel)
endif(MedReader_BUILD_PARALLEL)

#
# Build the ParaView Plugin
#
if(MedReader_BUILD_ParaView_Plugin)
    add_subdirectory(ParaViewPlugin)
endif(MedReader_BUILD_ParaView_Plugin)


	
