#
# Find the native SZIP includes and library
#
# SZIP_INCLUDE_DIR - where to find SZIP.h, etc.
# SZIP_LIBRARIES   - List of fully qualified libraries to link against when using SZIP.
# SZIP_FOUND       - Do not attempt to use SZIP if "no" or undefined.

FIND_PATH(SZIP_INCLUDE_DIR szlib.h
  ${MED_FULL_PACK_DIR}/ThirdPart/sziplib/include
  /local00/dossiers/reader_med/MedFullPackage/ThirdPart/sziplib/include
  /usr/local/include
  /usr/include
)

FIND_LIBRARY(SZIP_LIBRARY sz
  /usr/lib
  /usr/local/lib
  ${MED_FULL_PACK_DIR}/ThirdPart/sziplib/lib
  /local00/dossiers/reader_med/MedFullPackage/ThirdPart/sziplib/lib
)

IF(SZIP_INCLUDE_DIR)
  IF(SZIP_LIBRARY)
    SET( SZIP_LIBRARIES ${SZIP_LIBRARY} )
    SET( SZIP_FOUND "YES" )
  ENDIF(SZIP_LIBRARY)
ENDIF(SZIP_INCLUDE_DIR)
