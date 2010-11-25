set(MEDREADER_HDRS)
set(EXTRACTGROUP_HDRS)

include(${MedReader_BINARY_DIR}/IO/vtkMedReader_IOKit.cmake)
include(${MedReader_BINARY_DIR}/Filtering/vtkExtractGroup_FilteringKit.cmake)

foreach(class ${VTK_MEDREADER_IO_CLASSES})
    set(MEDREADER_HDRS ${MEDREADER_HDRS} ${VTK_MEDREADER_IO_HEADER_DIR}/${class}.h)
endforeach(class)

foreach(class ${VTK_MEDREADER_IO_CLASSES_ABSTRACT})
    set_source_files_properties(${VTK_MEDREADER_IO_HEADER_DIR}/${class}.h PROPERTIES ABSTRACT 1)
endforeach(class)

foreach(class ${VTK_MEDREADER_IO_CLASSES_WRAP_EXCLUDE})
    set_source_files_properties(${VTK_MEDREADER_IO_HEADER_DIR}/${class}.h PROPERTIES WRAP_EXCLUDE 1)
endforeach(class)

foreach(class ${VTK_EXTRACTGROUP_Filtering_CLASSES})
    set(EXTRACTGROUP_HDRS ${EXTRACTGROUP_HDRS} ${VTK_EXTRACTGROUP_Filtering_HEADER_DIR}/${class}.h)
endforeach(class)

SET(SM_SRCS
    vtkSMMedReaderProxy.cxx 
    vtkSMExtractGroupProxy.cxx
    vtkSMInformationKeyDomain.cxx
    vtkSMInformationKeyDomainHelper.cxx
    vtkPVServerInformationKey.cxx
    )

IF(PARAVIEW_BUILD_QT_GUI)

    QT4_WRAP_UI(CLIENT_UI_SRCS
        Resources/UI/MedReaderPanel.ui
        Resources/UI/ExtractGroupPanel.ui
    )

    # create a plugin that implements an object panel for the reader
    SET(CLIENT_SRCS
        pqMedReaderPanel.cxx
        pqExtractGroupPanel.cxx
    )

    QT4_WRAP_CPP(MOC_SRCS
        pqMedReaderPanel.h
        pqExtractGroupPanel.h
    )

    ADD_PARAVIEW_OBJECT_PANEL(MEDREADER_IFACES MEDREADER_IFACE_SRCS
        CLASS_NAME pqMedReaderPanel
        XML_NAME MedReader
        XML_GROUP sources
    )
        
    ADD_PARAVIEW_OBJECT_PANEL(EXTRACTGROUP_IFACES EXTRACTGROUP_IFACE_SRCS
        CLASS_NAME pqExtractGroupPanel
        XML_NAME ExtractGroup
        XML_GROUP filters
    )
    
    # create the med reader plugin itself
    ADD_PARAVIEW_PLUGIN(MedReaderPlugin "3.0"
        SERVER_MANAGER_SOURCES ${MEDREADER_HDRS} ${EXTRACTGROUP_HDRS} ${SM_SRCS}
        SERVER_MANAGER_XML Resources/MedReaderServer.xml Resources/ExtractGroupServer.xml
        GUI_INTERFACES ${MEDREADER_IFACES} ${EXTRACTGROUP_IFACES}
        GUI_RESOURCES Resources/pqMedReader.qrc Resources/pqExtractGroup.qrc
        GUI_SOURCES ${CLIENT_SRCS}
              ${CLIENT_UI_SRCS}
              ${MOC_SRCS}
              ${MEDREADER_IFACE_SRCS}
              ${EXTRACTGROUP_IFACE_SRCS}
    )
        
ELSE(PARAVIEW_BUILD_QT_GUI)
          
    # create the med reader plugin itself
    ADD_PARAVIEW_PLUGIN(MedReaderPlugin "3.0"
        SERVER_MANAGER_SOURCES ${MEDREADER_HDRS} ${EXTRACTGROUP_HDRS} ${SM_SRCS}
        SERVER_MANAGER_XML Resources/MedReaderServer.xml Resources/ExtractGroupServer.xml
    )
    
ENDIF(PARAVIEW_BUILD_QT_GUI)

TARGET_LINK_LIBRARIES(MedReaderPlugin vtkMedReader vtkExtractGroup)

# Install
INSTALL(
    TARGETS MedReaderPlugin
    RUNTIME DESTINATION .
    LIBRARY DESTINATION .
    ARCHIVE DESTINATION .
)




