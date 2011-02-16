PROJECT( ParaMEDCorba )

  CMAKE_MINIMUM_REQUIRED( VERSION 2.6 )
  ## !!! This definition corrupts wrapping process
  #SET( LIBRARY_OUTPUT_PATH ${ParaMEDCorba_BINARY_DIR}/bin CACHE INTERNAL "Single place for output" )
  #SET( EXECUTABLE_OUTPUT_PATH ${ParaMEDCorba_BINARY_DIR}/bin  CACHE INTERNAL "Single place for output" )
  MARK_AS_ADVANCED(
    LIBRARY_OUTPUT_PATH
    EXECUTABLE_OUTPUT_PATH
  )

  FIND_PACKAGE( ParaView REQUIRED )

  IF( ParaView_FOUND )
    
    INCLUDE( ${PARAVIEW_USE_FILE} )

    SET( PARACORBAPLUG_SRCS
      ${ParaMEDCorba_SOURCE_DIR}/vtkParaMEDCorbaSource.cxx
      )

    ADD_LIBRARY (ParaMEDMEM2VTK SHARED VTKMEDCouplingMeshClient.cxx VTKMEDCouplingUMeshClient.cxx VTKMEDCouplingCMeshClient.cxx
      VTKMEDCouplingFieldClient.cxx VTKParaMEDFieldClient.cxx VTKMEDCouplingMultiFieldsClient.cxx )
    
    ADD_PARAVIEW_PLUGIN( ParaMEDCorbaPlugin "1.0"
      SERVER_MANAGER_XML ParaMEDCorbaServerManager.xml
      GUI_RESOURCE_FILES
      ParaMEDCorbaUserInterfaceSources.xml
      SERVER_MANAGER_SOURCES ${PARACORBAPLUG_SRCS} )
    
    SET(flags)
    FOREACH(f ${CORBA_INCLUDES} ${CORBA_CXXFLAGS})
      SET(flags "${flags} ${f}")
    ENDFOREACH(f ${CORBA_INCLUDES} ${CORBA_CXXFLAGS})
    
    INCLUDE_DIRECTORIES(${MED_ROOT_DIR}/include/salome ${KERNEL_ROOT_DIR}/include/salome)
    SET_TARGET_PROPERTIES(ParaMEDMEM2VTK PROPERTIES COMPILE_FLAGS "${flags}")
    SET_TARGET_PROPERTIES(ParaMEDCorbaPlugin PROPERTIES COMPILE_FLAGS "${flags}")

    TARGET_LINK_LIBRARIES(ParaMEDMEM2VTK
      ${MED_ROOT_DIR}/lib/salome/libSalomeIDLMED.so
      ${KERNEL_ROOT_DIR}/lib/salome/libSalomeIDLKernel.so
      )
    TARGET_LINK_LIBRARIES(ParaMEDCorbaPlugin
      vtkFiltering
      vtkCommon
      ParaMEDMEM2VTK
      ${MED_ROOT_DIR}/lib/salome/libSalomeIDLMED.so
      ${KERNEL_ROOT_DIR}/lib/salome/libSalomeIDLKernel.so
      ${CORBA_LIBS}
      )
    
    INSTALL(TARGETS ParaMEDCorbaPlugin DESTINATION lib/paraview)

    INSTALL(TARGETS ParaMEDMEM2VTK DESTINATION lib/salome)
    
    ADD_EXECUTABLE( testParaMEDCorba
      ${ParaMEDCorba_SOURCE_DIR}/testParaMEDCorba.cxx
      )
    # SET_TARGET_PROPERTIES(testCorbaPlugin PROPERTIES COMPILE_FLAGS "${flags}")
    
    TARGET_LINK_LIBRARIES( testParaMEDCorba
      ParaMEDCorbaPlugin
      )
    
    INSTALL(FILES ParaMEDCorbaServerManager.xml DESTINATION share)
    
  
  ENDIF( ParaView_FOUND )

