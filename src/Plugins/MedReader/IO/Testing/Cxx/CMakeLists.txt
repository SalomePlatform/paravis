include_directories(../..)

set(TestMedReader_SRCS
    TestMedReader.cxx
)

set(TestMedPolyhedron_SRCS
    TestMedPolyhedron.cxx
)

set(TestMedReadPolyhedron_SRCS
    TestMedReadPolyhedron.cxx
)

set(TestMedDescendingPolyhedron_SRCS
    TestMedDescendingPolyhedron.cxx
)

set(TestMedReadDescendingPolyhedron_SRCS
    TestMedReadDescendingPolyhedron.cxx
)

set(TestMedWriteOcta12_SRCS
    TestMedWriteOcta12.cxx
)

add_executable(TestMedReader ${TestMedReader_SRCS})
add_executable(TestMedPolyhedron ${TestMedPolyhedron_SRCS})
add_executable(TestMedReadPolyhedron ${TestMedReadPolyhedron_SRCS})
add_executable(TestMedDescendingPolyhedron ${TestMedDescendingPolyhedron_SRCS})
add_executable(TestMedReadDescendingPolyhedron ${TestMedReadDescendingPolyhedron_SRCS})
add_executable(TestMedWriteOcta12 ${TestMedWriteOcta12_SRCS})

set(VTK_LIBS vtkMedReader vtkRendering)
if(${HDF5_IS_PARALLEL})
  set(VTK_LIBS ${VTK_LIBS} vtkParallel)
endif(${HDF5_IS_PARALLEL})

target_link_libraries(TestMedReader ${VTK_LIBS} ${MED_LIBRARIES})
target_link_libraries(TestMedPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedReadPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedDescendingPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedReadDescendingPolyhedron ${MED_LIBRARIES})
target_link_libraries(TestMedWriteOcta12 ${MED_LIBRARIES})

