#test if we have not yet included ParaView and/or VTK configuration files
IF(NOT VTK_FOUND)
    FIND_PACKAGE(VTK REQUIRED)
    INCLUDE(${VTK_USE_FILE})
ENDIF(NOT VTK_FOUND)

set(MedReader_CLASSES
    vtkMedReader
    vtkMedDriver
    vtkMedDriver30
    vtkMedFactory
    vtkMedFile
    vtkMedMesh
    vtkMedGrid
    vtkMedRegularGrid
    vtkMedPolarGrid
    vtkMedCartesianGrid
    vtkMedCurvilinearGrid
    vtkMedUnstructuredGrid
    vtkMedFamily
    vtkMedGroup
    vtkMedField
    vtkMedEntityArray
    vtkMedProfile
    vtkMedLocalization
    vtkMedFieldOverEntity
    vtkMedFieldStep
    vtkMedUtilities
    vtkMedString
    vtkMedIntArrayInternal
    vtkMedIntArray
    vtkMedFamilyOnEntity
    vtkMedInterpolation
    vtkMedFraction
    vtkMedFilter
    vtkMedFieldOnProfile
    vtkExtractGroup
    vtkMedSelection
    vtkMedLink
    vtkMedFamilyOnEntityOnProfile
)

set(MedReader_SRCS)
set(MedReader_HDRS)

foreach(class ${MedReader_CLASSES})
    set(MedReader_SRCS ${MedReader_SRCS} ${class}.cxx)
    set(MedReader_HDRS ${MedReader_HDRS} ${class}.h)
endforeach(class)

set_source_files_properties(
  vtkMedGrid
  vtkMedRegularGrid
  vtkMedUtilities
  PROPERTIES ABSTRACT 1
)

set_source_files_properties(
  vtkMedIntArrayInternal
  vtkMedUtilities
  PROPERTIES WRAP_EXCLUDE 1
)

add_library(vtkMedReader ${MedReader_SRCS})

set(VTK_LIBS vtkIO)
if(${HDF5_IS_PARALLEL})
  set(VTK_LIBS ${VTK_LIBS} vtkParallel)
endif(${HDF5_IS_PARALLEL})

target_link_libraries(vtkMedReader ${MED_LIBRARIES} ${VTK_LIBS})

set(MedReader_BUILD_Testing ${VTK_BUILD_TESTING} CACHE BOOL "Build tests for the MedReader filter")

if(MedReader_BUILD_Testing)
    add_subdirectory(Testing)
endif(MedReader_BUILD_Testing)

install(
    TARGETS vtkMedReader
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib/salome
    ARCHIVE DESTINATION .
    )
    
install(FILES ${MedReader_HDRS} DESTINATION include)
        
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
  "MedReader_IO" 
  "MEDREADER_IO" 
  ${CMAKE_CURRENT_BINARY_DIR} 
  "${MedReader_SRCS}"
)
