FIND_PACKAGE(VTK REQUIRED)
INCLUDE(${VTK_USE_FILE})

set(PMedReader_SRCS
    vtkPMedReader.cxx
)

add_library(vtkPMedReader ${PMedReader_SRCS})

target_link_libraries(vtkPMedReader vtkMedReader vtkParallel)

#set(PMedReader_BUILD_Testing ON CACHE BOOL "Build tests for the PMedReader filter")
#
#if(PMedReader_BUILD_Testing)
#    add_subdirectory(Testing)
#endif(PMedReader_BUILD_Testing)

# -----------------------------------------------------------------------------
# This make it easy for other projects to get the list of files etc. in this
# kit.
# -----------------------------------------------------------------------------
# needed by vtkExportKit.cmake
include(${VTK_CMAKE_DIR}/vtkExportKit.cmake)
IF(NOT VTK_INSTALL_NO_DEVELOPMENT)
  STRING(REGEX REPLACE "^/" "" VTK_INSTALL_PACKAGE_DIR_CM24 "${VTK_INSTALL_PACKAGE_DIR}")
ENDIF(NOT VTK_INSTALL_NO_DEVELOPMENT)

vtk_export_kit2(
  "MedReader_Parallel" 
  "MEDREADER_PARALLEL" 
  ${CMAKE_CURRENT_BINARY_DIR} 
  "${PMedReader_SRCS}"
)
