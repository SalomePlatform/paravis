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

IF (PYTHON_EXECUTABLE)
  FOREACH ( tfile
      A1	
      A2
      A3
      A4
      A5      
      A7
      A8
      A9
      B0     
      B2
      E0
      E1
      E2      
      E4
      E5
      E6
      E7
      E8
      E9      
      F1
      F2
      F3
      F4
      F5
      F6
      F7
      F8            
      G1
      G2
      )
    ADD_TEST(SCALARMAP_${tfile} ${PYTHON_EXECUTABLE} $ENV{PARAVIS_SRC_DIR}/test/VisuPrs/ScalarMap/${tfile}.py)
  ENDFOREACH( tfile )
ENDIF (PYTHON_EXECUTABLE)











