# TableReader plugin consists of two parts:
# 1. Table reader - reads tables in Post-Pro specific format
# 2. "Table To 3D" filter - creates 3D representation of a table

cmake_minimum_required(VERSION 2.4)
if(COMMAND cmake_policy)
	 cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(TableReader)

# Find ParaView
FIND_PACKAGE(ParaView REQUIRED)
if(NOT ParaView_FOUND)
    MESSAGE(FATAL_ERROR "Please locate ParaView." )
ENDIF(NOT ParaView_FOUND)
INCLUDE(${PARAVIEW_USE_FILE})

# Standard CMake option for building libraries shared or static by default.
OPTION(BUILD_SHARED_LIBS
       "Build with shared libraries."
       ${VTK_BUILD_SHARED_LIBS})

# Add subdirectories
INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/Reader ${CMAKE_CURRENT_SOURCE_DIR}/TableTo3DFilter)
ADD_SUBDIRECTORY(Reader)
ADD_SUBDIRECTORY(TableTo3DFilter)
ADD_SUBDIRECTORY(ParaViewPlugin)