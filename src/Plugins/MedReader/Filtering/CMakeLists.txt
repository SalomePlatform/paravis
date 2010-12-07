set(ExtractGroup_SRCS
    vtkExtractGroup.cxx
)

add_library(vtkExtractGroup ${ExtractGroup_SRCS})

target_link_libraries(vtkExtractGroup vtkFiltering vtkMedReader)

install(
        TARGETS vtkExtractGroup
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        ARCHIVE DESTINATION .
        )

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
  "ExtractGroup_Filtering" 
  "EXTRACTGROUP_Filtering" 
  ${CMAKE_CURRENT_BINARY_DIR} 
  "${ExtractGroup_SRCS}"
)
