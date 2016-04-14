# Copyright (C) 2015-2016  CEA/DEN, EDF R&D
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
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

SET(COMPONENT_NAME MEDREADER)
SET(TIMEOUT        60)

SET(TEST_NUMBERS 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20)
set(BASELINES_DIR "Baselines")

FOREACH(tfile ${TEST_NUMBERS})
  SET(TEST_NAME MEDREADER_${tfile})
  ADD_TEST(${TEST_NAME} python testMEDReader${tfile}.py -B ${BASELINES_DIR})
  SET_TESTS_PROPERTIES(${TEST_NAME} PROPERTIES LABELS "${COMPONENT_NAME}" TIMEOUT ${TIMEOUT})
ENDFOREACH()
