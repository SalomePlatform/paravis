SET(READER_SRCS
    TableParser.cxx
    vtkVisuTableReader.cxx
    )

ADD_LIBRARY(vtkVisuTableReader ${READER_SRCS})

INSTALL(
    TARGETS vtkVisuTableReader
    RUNTIME DESTINATION .
    LIBRARY DESTINATION lib/salome
    ARCHIVE DESTINATION .
    )