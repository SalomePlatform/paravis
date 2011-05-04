PROJECT(ElevationSurface)
cmake_minimum_required(VERSION 2.6)

cmake_policy(SET CMP0003 NEW)

FIND_PACKAGE(ParaView REQUIRED)
INCLUDE(${PARAVIEW_USE_FILE})

SET(SRCS
    vtkElevationSurfaceFilter.cxx 
    )

ADD_PARAVIEW_PLUGIN(ElevationSurface "1.0"
	SERVER_MANAGER_XML ElevationSurface.xml
	GUI_RESOURCE_FILES ElevationSurfaceClient.xml
	SERVER_MANAGER_SOURCES ${SRCS})

install(TARGETS ${PROJECT_NAME} 
   RUNTIME DESTINATION .
   LIBRARY DESTINATION lib/paraview
   ARCHIVE DESTINATION .
   )

