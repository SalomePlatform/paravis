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

SET(AM_CPPFLAGS ${AM_CPPFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)
SET(AM_CXXFLAGS ${AM_CXXFLAGS} -DHAVE_SALOME_CONFIG -I${KERNEL_ROOT_DIR}/include/salome -include SALOMEconfig.h)

#Executable
SET(bin_PROGRAMS
  vtkWrapIDL
  )

SET(dist_vtkWrapIDL_SOURCES
  ${CMAKE_SOURCE_DIR}/src/ParaView/vtkParse.tab.c
  ${CMAKE_CURRENT_SOURCE_DIR}/vtkWrapIDL.c
  )

SET(vtkWrapIDL_CPPFLAGS
  -I${CMAKE_SOURCE_DIR}/src/ParaView
  -I${CMAKE_BINARY_DIR}/idl
  ${PARAVIEW_INCLUDES}
  ${VTK_INCLUDES}
  )

INCLUDE(wrap.cmake)

SET(BASEIDL_FILES
  PARAVIS_Gen_Types.idl
  PARAVIS_Gen.idl
  )

SET(BASEIDL_SK_FILES
  PARAVIS_Gen_TypesSK.cc
  PARAVIS_GenSK.cc
  )

SET(IDL_FILES
  ${BASEIDL_FILES}
  ${WRAP_IDL}
  )

# This variable defines the files to be installed
SET(dist_salomeidl_DATA
  ${IDL_FILES}
  )

# PARAVIS idl common library
SET(lib_LTLIBRARIES
  libSalomeIDLPARAVIS.la
  )

# Sources built from idl files
SET(nodist_libSalomeIDLPARAVIS_la_SOURCES
  ${BASEIDL_SK_FILES}
  ${WRAP_SK_FILES}
  )

SET(libSalomeIDLPARAVIS_la_CPPFLAGS
  -I${CMAKE_BINARY_DIR}/salome_adm/unix
  -I${CMAKE_BINARY_DIR}/idl
  ${CORBA_CXXFLAGS}
  ${CORBA_INCLUDES}
  ${KERNEL_CXXFLAGS}
  )

SET(libSalomeIDLPARAVIS_la_LIBADD
  ${CORBA_LIBS}
  )

# These variables defines the building process of CORBA files
SET(OMNIORB_IDL
  ${OMNIORB_IDL}
  )

SET(OMNIORB_IDLCXXFLAGS
  ${OMNIORB_IDLCXXFLAGS}
  )

SET(OMNIORB_IDLPYFLAGS
  ${OMNIORB_IDLPYFLAGS}
  -I${KERNEL_ROOT_DIR}/idl/salome
  )

SET(IDLCXXFLAGS
  -bcxx
  ${IDLCXXFLAGS}
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -I${KERNEL_ROOT_DIR}/idl/salome
  -I${CMAKE_BINARY_DIR}/salome_adm/unix
  )

SET(IDLPYFLAGS
  -bpythonIDL
  -nf
  -p${CMAKE_CURRENT_SOURCE_DIR}
  -I${CMAKE_CURRENT_SOURCE_DIR}
  -I${KERNEL_ROOT_DIR}/idl/salome
  )

# potential problem on parallel make on the following - multiple outputs
SET(SUFFIXES
  .idl
  .hh
  SK.cc
  )

SET(amname libSalomeIDLPARAVIS_la)
SET(name SalomeIDLPARAVIS)

SET(srcs ${${amname}_SOURCES} ${dist_${amname}_SOURCES})

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

SET(amname vtkWrapIDL)

SET(name "${amname}_exe")
SET(srcs ${${amname}_SOURCES} ${dist_${amname}_SOURCES})

ADD_EXECUTABLE(${name} ${srcs})
ADD_DEPENDENCIES(${name} generate_txt)
  
SET(flags)
FOREACH(f ${AM_CPPFLAGS} ${AM_CXXFLAGS} ${PLATFORM_CPPFLAGS} ${PTHREAD_CFLAGS} ${${amname}_CPPFLAGS} ${${amname}_CXXFLAGS} ${${amname}_CFLAGS})
  SET(flags "${flags} ${f}")
ENDFOREACH(f ${var})
SET_TARGET_PROPERTIES(${name} PROPERTIES COMPILE_FLAGS "${flags}")
  
SET(libadd ${PLATFORM_LIBADD} ${PLATFORM_LDFLAGS} ${${amname}_LIBADD} ${${amname}_LDADD} ${${amname}_LDFLAGS})
TARGET_LINK_LIBRARIES(${name} ${libadd})

SET(PERMS)
SET(PERMS ${PERMS} OWNER_READ OWNER_WRITE OWNER_EXECUTE)
SET(PERMS ${PERMS} GROUP_READ GROUP_EXECUTE)
SET(PERMS ${PERMS} WORLD_READ WORLD_EXECUTE)
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/${name} DESTINATION bin/salome PERMISSIONS ${PERMS} RENAME ${amname})

FOREACH(input ${IDL_FILES})
  STRING(REGEX REPLACE ".idl" "" base ${input})
  SET(src ${CMAKE_CURRENT_BINARY_DIR}/${base}SK.cc)
  SET(outputs ${src})
  SET(dynsrc ${CMAKE_CURRENT_BINARY_DIR}/${base}DynSK.cc)
  SET(outputs ${outputs} ${dynsrc})
  SET(inc ${CMAKE_CURRENT_BINARY_DIR}/${base}.hh)
  SET(outputs ${outputs} ${inc})
  IF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${input})
    SET(input ${CMAKE_CURRENT_SOURCE_DIR}/${input})
  ELSE(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${input})
    SET(input ${CMAKE_CURRENT_BINARY_DIR}/${input})
  ENDIF(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${input})
  SET(flags ${IDLCXXFLAGS} ${OMNIORB_IDLCXXFLAGS})
  ADD_CUSTOM_COMMAND(
    OUTPUT ${outputs}
    COMMAND ${OMNIORB_IDL} ${flags} ${input}
    MAIN_DEPENDENCY ${input}
    )
  INSTALL(FILES ${input} DESTINATION idl/salome)
  SET(IDL_HEADER ${CMAKE_CURRENT_BINARY_DIR}/${base}.hh)
  INSTALL(FILES ${IDL_HEADER} DESTINATION include/salome)
  INSTALL(CODE "SET(IDL_FILE ${input})")
  INSTALL(CODE "SET(DIR lib/python${PYTHON_VERSION}/site-packages/salome)")
  INSTALL(CODE "SET(CMAKE_CURRENT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR})")
  INSTALL(CODE "SET(OMNIORB_IDL_PYTHON ${OMNIORB_IDL_PYTHON})")
  # --
  SET(flags)
  FOREACH(f ${IDLPYFLAGS})
    SET(flags "${flags} ${f}")
  ENDFOREACH(f ${IDLPYFLAGS})
  SET(IDLPYFLAGS ${flags})
  INSTALL(CODE "SET(IDLPYFLAGS ${IDLPYFLAGS})")
  # --
  INSTALL(SCRIPT ${KERNEL_ROOT_DIR}/salome_adm/cmake_files/install_python_from_idl.cmake)

ENDFOREACH(input ${IDL_FILES})
            
