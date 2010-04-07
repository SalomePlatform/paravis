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

SET(salomeinclude_HEADERS
  PARAVIS_Engine_i.hh
  )

SET(dist_libPARAVISEngine_la_SOURCES
  PARAVIS_Engine_i.cc
  )

SET(libPARAVISEngine_la_CPPFLAGS
  ${KERNEL_CXXFLAGS}
  ${CORBA_CXXFLAGS}
  ${CORBA_INCLUDES}
  -I${CMAKE_BINARY_DIR}/salome_adm/unix
  -I${CMAKE_BINARY_DIR}/idl
  )

SET(libPARAVISEngine_la_LDFLAGS
  ${KERNEL_LDFLAGS}
  ${SalomeIDLKernel}
  ${SalomeNS}
  ${SalomeContainer}
  ${CORBA_LIBS}
  SalomeIDLPARAVIS
  )

FOREACH(dir ${SUBDIRS})
  ADD_SUBDIRECTORY(${dir})
ENDFOREACH(dir ${SUBDIRS})

SET(amname libPARAVISEngine_la)
SET(name PARAVISEngine)

SET(srcs ${${amname}_SOURCES} ${dist_${amname}_SOURCES})

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
  
INSTALL(FILES PARAVIS_Engine_i.hh DESTINATION include/salome)
