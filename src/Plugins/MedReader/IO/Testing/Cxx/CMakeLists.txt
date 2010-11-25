set(TestMedReader_SRCS
    TestMedReader.cxx
)

add_executable(TestMedReader ${TestMedReader_SRCS})

target_link_libraries(TestMedReader vtkMedReader vtkRendering ${MED_LIBRARIES})