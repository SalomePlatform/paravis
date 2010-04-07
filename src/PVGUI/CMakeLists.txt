#  Copyright (C) 2007-2010  CEA/DEN, EDF R&D, OPEN CASCADE
#
#  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
#  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
#
#  This library is free software; you can redistribute it and/or
#  modify it under the terms of the GNU Lesser General Public
#  License as published by the Free Software Foundation; either
#  version 2.1 of the License.
#
#  This library is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#  Lesser General Public License for more details.
#
#  You should have received a copy of the GNU Lesser General Public
#  License along with this library; if not, write to the Free Software
#  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
#  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(VERSION_INFO 0.0.0)
SET(SOVERSION_INFO 0)
SET(SUBDIRS)
SET(AM_CPPFLAGS)
SET(AM_CXXFLAGS)
SET(LDADD)

SET(AM_CPPFLAGS ${AM_CPPFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
SET(AM_CXXFLAGS ${AM_CXXFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)

IF(WITH_VISU)
  SET(VISU_PARAVIS_LIBS
    -lSalomeIDLVISU
    )
  
  SET(VISU_PARAVIS_DEFINES
    -DWITH_VISU
    )
ENDIF(WITH_VISU)

#Executable
SET(bin_PROGRAMS
  vtkWrapIDL_HH
  vtkWrapIDL_CC
  )

SET(dist_vtkWrapIDL_HH_SOURCES
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParse.tab.c
  ${CMAKE_SOURCE_DIR}/idl/vtkWrapIDL.c
  )

SET(dist_vtkWrapIDL_CC_SOURCES
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParse.tab.c
  ${CMAKE_SOURCE_DIR}/idl/vtkWrapIDL.c
  )

SET(vtkWrapIDL_HH_CPPFLAGS
  -DIDL_I_HH
  -I${CMAKE_CURRENT_SOURCE_DIR}/../ParaView
  -I${CMAKE_BINARY_DIR}/idl
  ${PARAVIEW_INCLUDES}
  ${VTK_INCLUDES}
  )

SET(vtkWrapIDL_CC_CPPFLAGS
  -DIDL_I_CC
  -I${CMAKE_CURRENT_SOURCE_DIR}/../ParaView
  -I${CMAKE_BINARY_DIR}/idl
  ${PARAVIEW_INCLUDES}
  ${VTK_INCLUDES}
  )

INCLUDE(wrap.cmake)

# Library sources
SET(dist_libPARAVIS_la_SOURCES
  PVGUI_Module.h
  PVGUI_Module_impl.h
  PVGUI_Module.cxx
  PVGUI_Module_actions.cxx
  PVGUI_Module_widgets.cxx
  PVGUI_ViewManager.h
  PVGUI_ViewManager.cxx
  PVGUI_ViewModel.h
  PVGUI_ViewModel.cxx
  PVGUI_ViewWindow.h
  PVGUI_ViewWindow.cxx
  PVGUI_OutputWindowAdapter.h
  PVGUI_OutputWindowAdapter.cxx
  PVGUI_Tools.h
  PVGUI_Tools.cxx
  PV_I.h
  PARAVIS_Gen_i.hh
  PV_Events.h
  PV_Tools.h
  PARAVIS_Gen_i.cc
  PV_Tools.cxx
  ${WRAP_IDL_I_HH}
  ${WRAP_IDL_I_CC}
  PARAVIS_CreateClass.cxx
  )

# MOC pre-processing
SET(MOC_FILES
  PVGUI_Module_moc.cxx
  PVGUI_ViewManager_moc.cxx
  PVGUI_ViewModel_moc.cxx
  PVGUI_ViewWindow_moc.cxx
  )

SET(nodist_libPARAVIS_la_SOURCES
  ${MOC_FILES}
  )

# additionnal compilation flags
# Note that ParaView uses HDF5
SET(libPARAVIS_la_CPPFLAGS
  -ftemplate-depth-32
  ${HDF5_INCLUDES}
  ${QT_INCLUDES}
  ${QT_ASSISTANT_INCLUDES}
  ${PARAVIEW_INCLUDES}
  ${VTK_INCLUDES}
  ${CAS_CPPFLAGS}
  ${PYTHON_INCLUDES}
  ${CAS_CPPFLAGS}
  ${CAS_CXXFLAGS}
  ${KERNEL_CXXFLAGS}
  ${CORBA_CXXFLAGS}
  ${CORBA_INCLUDES}
  ${GUI_CXXFLAGS}
  ${VISU_CXXFLAGS}
  ${MED_CXXFLAGS}
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -I${CMAKE_BINARY_DIR}/idl
  -I${CMAKE_CURRENT_SOURCE_DIR}/../../idl
  -I${CMAKE_BINARY_DIR}/salome_adm/unix
  ${VISU_PARAVIS_DEFINES}
  )

# additionnal linkage flags
# Note that ParaView uses HDF5
SET(libPARAVIS_la_LDFLAGS
  ${PYTHON_LIBS}
  ${QT_MT_LIBS}
  ${HDF5_LIBS}
  ${QT_ASSISTANT_LIBS}
  ${PARAVIEW_LIBS}
  SalomeIDLPARAVIS
  ${KERNEL_LDFLAGS}
  ${SalomeNS}
  ${SalomeDSClient}
  ${SalomeContainer}
  ${SalomeLifeCycleCORBA}
  ${GUI_LDFLAGS}
  ${SalomeApp}
  ${LightApp}
  ${VISU_LDFLAGS}
  ${VISU_PARAVIS_LIBS}
  )

SET(libPARAVIS_la_LIBADD
  ${SalomeIDLKernel}
  ${SalomeHDFPersist}
  ${SalomeGenericObj}
  ${SalomeContainer}
  ${SalomeCommunication}
  ${SalomeNS}
  ${TOOLSDS}
  ${OpUtil}
  ${SalomeLifeCycleCORBA}
  ${Event}
  ${SalomeApp}
  ${SalomeSession}
  ${qtx}
  ${LightApp}
  )

#	
# resources files
SET(nodist_salomeres_DATA
  PARAVIS_images.qm
  PARAVIS_msg_en.qm
  )

FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})

SET(amname libPARAVIS_la)
SET(name PARAVIS)

SET(srcs)
FOREACH(src ${${amname}_SOURCES} ${dist_${amname}_SOURCES})
  SET(srcs ${srcs} ${src})
ENDFOREACH(src ${${amname}_SOURCES} ${dist_${amname}_SOURCES})

FOREACH(f ${nodist_${amname}_SOURCES})
  SET(srcs ${srcs} ${CMAKE_CURRENT_BINARY_DIR}/${f})
ENDFOREACH(f ${nodist_${amname}_SOURCES})

ADD_LIBRARY(${name} SHARED ${srcs})

SET(flags)
FOREACH(f ${AM_CPPFLAGS} ${AM_CXXFLAGS} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${${amname}_CPPFLAGS} ${${amname}_CXXFLAGS} ${${amname}_CFLAGS})
  SET(flags "${flags} ${f}")
ENDFOREACH(f ${var})

SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "${flags}")
SET_TARGET_PROPERTIES(${name} PROPERTIES VERSION ${VERSION_INFO} SOVERSION ${SOVERSION_INFO})

SET(libadd ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS} ${${amname}_LIBADD} ${${amname}_LDADD} ${${amname}_LDFLAGS})
TARGET_LINK_LIBRARIES(${name} ${libadd})
INSTALL(TARGETS ${name} DESTINATION lib/salome)

FOREACH(amname ${bin_PROGRAMS})
  SET(name "${amname}_exe")
  SET(srcs ${${amname}_SOURCES} ${dist_${amname}_SOURCES})
  SET(l ${nodist_${amname}_SOURCES})
  FOREACH(f ${nodist_${amname}_SOURCES})
    SET(srcs ${srcs} ${CMAKE_CURRENT_BINARY_DIR}/${f})
  ENDFOREACH(f ${l})
  
  ADD_EXECUTABLE(${name} ${srcs})
  
  SET(flags)
  FOREACH(f ${AM_CPPFLAGS} ${AM_CXXFLAGS} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${${amname}_CPPFLAGS} ${${amname}_CXXFLAGS} ${${amname}_CFLAGS})
    SET(flags "${flags} ${f}")
  ENDFOREACH(f ${var})
  
  SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "${flags}")
  
  SET(libadd ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS} ${${amname}_LIBADD} ${${amname}_LDADD} ${${amname}_LDFLAGS} ${LDADD})
  
  TARGET_LINK_LIBRARIES(${name} ${libadd})
  
  SET(PERMS)
  SET(PERMS ${PERMS} OWNER_READ OWNER_WRITE OWNER_EXECUTE)
  SET(PERMS ${PERMS} GROUP_READ GROUP_EXECUTE)
  SET(PERMS ${PERMS} WORLD_READ WORLD_EXECUTE)
  INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name} DESTINATION bin/salome PERMISSIONS ${PERMS} RENAME ${amname})
  
ENDFOREACH(amname ${bin_PROGRAMS})

FOREACH(output ${MOC_FILES})
  STRING(REGEX REPLACE _moc.cxx .h input ${output})
  
  SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${input})
  SET(output ${CMAKE_CURRENT_BINARY_DIR}/${output})
  ADD_CUSTOM_COMMAND(
    OUTPUT ${output}
    COMMAND ${QT_MOC_EXECUTABLE} ${MOC_FLAGS} ${input} -o ${output}
    MAIN_DEPENDENCY ${input}
    )
ENDFOREACH(output ${MOC_FILES})
            
FOREACH(f ${nodist_salomeres_DATA})
  STRING(REGEX REPLACE .qm .ts input ${f})
  SET(input ${CMAKE_CURRENT_SOURCE_DIR}/resources/${input})
  SET(output ${CMAKE_CURRENT_BINARY_DIR}/${f})
  EXECUTE_PROCESS(COMMAND ${QT_LRELEASE_EXECUTABLE} ${input} -qm ${output})
  INSTALL(FILES ${output} DESTINATION share/salome/resources/paravis)
ENDFOREACH(f ${nodist_salomeres_DATA})
