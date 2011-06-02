# Copyright (C) 2010-2011  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
#
# See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
#

SET(WRAP_IDL_I_HH)
SET(WRAP_IDL_I_CC)

IF(EXISTS ${CMAKE_BINARY_DIR}/wrapfiles.txt)
 EXECUTE_PROCESS(
  COMMAND cat ${CMAKE_BINARY_DIR}/wrapfiles.txt
  OUTPUT_VARIABLE WRAP_LIST_FULL
 )

 STRING(REGEX  MATCHALL "[^\n]+" WRAP_LIST_REG ${WRAP_LIST_FULL})
 FOREACH(STR ${WRAP_LIST_REG})

  SEPARATE_ARGUMENTS(STR)
  LIST(LENGTH STR WRAP_LEN)
  SET(DEP_HH)
  SET(DEP_CC)
 
  LIST(GET STR 0 VAL)

  IF(WRAP_LEN GREATER 1)
   MATH(EXPR WRAP_LEN1 "${WRAP_LEN} - 1" )

   FOREACH(IND RANGE 1 ${WRAP_LEN1})
    LIST(GET STR ${IND} DEP_VAL)
    SET(DEP_HH ${DEP_HH} PARAVIS_Gen_${DEP_VAL}_i.hh)
    SET(DEP_CC ${DEP_CC} PARAVIS_Gen_${DEP_VAL}_i.cc)
   ENDFOREACH(IND RANGE 1 ${WRAP_LEN1})

  ENDIF(WRAP_LEN GREATER 1)

  SET(WRAP_IDL_I_HH ${WRAP_IDL_I_HH} PARAVIS_Gen_${VAL}_i.hh)
  SET(WRAP_IDL_I_CC ${WRAP_IDL_I_CC} PARAVIS_Gen_${VAL}_i.cc)

  ADD_CUSTOM_COMMAND(
   OUTPUT PARAVIS_Gen_${VAL}_i.hh
   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vtkWrapIDL_HH ${PARAVIEW_INCLUDE_DIRS}/${VAL}.h ${CMAKE_BINARY_DIR}/idl/hints 0 PARAVIS_Gen_${VAL}_i.hh
   DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/vtkWrapIDL_HH ${PARAVIEW_INCLUDE_DIRS}/${VAL}.h ${CMAKE_BINARY_DIR}/idl/hints ${DEP_HH}
  ) 

  ADD_CUSTOM_COMMAND(
   OUTPUT PARAVIS_Gen_${VAL}_i.cc
   COMMAND ${CMAKE_CURRENT_BINARY_DIR}/vtkWrapIDL_CC ${PARAVIEW_INCLUDE_DIRS}/${VAL}.h ${CMAKE_BINARY_DIR}/idl/hints 0 PARAVIS_Gen_${VAL}_i.cc
   DEPENDS PARAVIS_Gen_${VAL}_i.hh ${CMAKE_CURRENT_BINARY_DIR}/vtkWrapIDL_CC ${PARAVIEW_INCLUDE_DIRS}/${VAL}.h ${CMAKE_BINARY_DIR}/idl/hints ${DEP_CC}
  )

 ENDFOREACH(STR ${WRAP_LIST_REG})
ENDIF(EXISTS ${CMAKE_BINARY_DIR}/wrapfiles.txt)

ADD_CUSTOM_COMMAND(
 OUTPUT PARAVIS_CreateClass.cxx
 COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/create_class.sh ${CMAKE_BINARY_DIR}
 DEPENDS ${CMAKE_BINARY_DIR}/wrapfiles.txt ${WRAP_IDL_I_HH}
)
ADD_CUSTOM_TARGET(generate_pvgui ALL DEPENDS ${CMAKE_BINARY_DIR}/wrapfiles.txt PARAVIS_CreateClass.cxx ${WRAP_IDL_I_HH} ${WRAP_IDL_I_CC})
