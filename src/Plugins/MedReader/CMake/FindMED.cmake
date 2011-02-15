# - Find MED library
# Find the MED includes and library
# This module defines
#  MED2HOME, a directory where MED was installed. This directory is used to help find trhe other values.
#  MED_INCLUDE_DIR, where to find med.h, etc.
#  MED_LIBRARIES, libraries to link against to use MED.
#  MED_FOUND, If false, do not try to use MED.
# also defined, but not for general use are
#  MED_LIBRARY, where to find the MED library.

SET(MED2HOME $ENV{MED2HOME} CACHE PATH "Path to the med install dir")

IF(NOT MED2HOME)
  FIND_PROGRAM(MDUMP mdump)
  IF(MDUMP)
    SET(MED2HOME ${MDUMP})
    GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
    GET_FILENAME_COMPONENT(MED2HOME ${MED2HOME} PATH)
  ENDIF(MDUMP)
ENDIF(NOT MED2HOME)

FIND_PATH(MED_INCLUDE_DIR med.h
  HINTS
  ${MED2HOME}/include
  PATHS
  /usr/local/include
  /usr/include
)

FIND_LIBRARY(MED_LIBRARY med
  HINTS
  ${MED_INCLUDE_DIR}/../lib
  ${MED2HOME}/lib
  PATHS
  /usr/local/lib  
  /usr/lib 
)

get_filename_component(MED_LIBRARY_DIR ${MED_LIBRARY} PATH)

FIND_LIBRARY(MEDC_LIBRARY medC
  HINTS
  ${MED_LIBRARY_DIR}
  ${MED2HOME}/lib
  PATHS
  /usr/local/lib  
  /usr/lib 
)

IF(MED_INCLUDE_DIR)
  IF(MED_LIBRARY)
    IF(MEDC_LIBRARY)
      SET(MED_LIBRARIES ${MED_LIBRARY} ${MEDC_LIBRARY} )
      SET( MED_FOUND "YES" )
    ENDIF(MEDC_LIBRARY)
  ENDIF(MED_LIBRARY)
ENDIF(MED_INCLUDE_DIR)

