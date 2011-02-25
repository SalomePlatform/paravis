# NavigationMode plugin.
# This is auto-start plugin providing navigation
# mode in 3D viewer in Post-Pro style.

CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
if(COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(NavigationMode)

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

QT4_WRAP_CPP(MOC_SRCS pqSetModeStarter.h)

ADD_PARAVIEW_AUTO_START(IFACES IFACE_SRCS CLASS_NAME pqSetModeStarter
                        STARTUP onStartup)

ADD_PARAVIEW_PLUGIN(NavigationMode "1.0" 
                    GUI_INTERFACES ${IFACES} 
                    SOURCES pqSetModeStarter.cxx ${MOC_SRCS} ${RCS_SRCS} ${IFACE_SRCS})

# Install
INSTALL(
    TARGETS NavigationMode
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib/paraview
    ARCHIVE DESTINATION .
)