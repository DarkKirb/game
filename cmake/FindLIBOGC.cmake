# - Try to find ctrulib
# Once done this will define
#  LIBCTRU_FOUND - System has ctrulib
#  LIBCTRU_INCLUDE_DIRS - The ctrulib include directories
#  LIBCTRU_LIBRARIES - The libraries needed to use ctrulib
#
# It also adds an imported target named `3ds::ctrulib`.
# Linking it is the same as target_link_libraries(target ${LIBCTRU_LIBRARIES}) and target_include_directories(target ${LIBCTRU_INCLUDE_DIRS})

# DevkitPro paths are broken on windows, so we have to fix those
macro(msys_to_cmake_path MsysPath ResultingPath)
    string(REGEX REPLACE "^/([a-zA-Z])/" "\\1:/" ${ResultingPath} "${MsysPath}")
endmacro()

if(NOT DEVKITPRO)
    msys_to_cmake_path("$ENV{DEVKITPRO}" DEVKITPRO)
endif()

set(LIBOGC_PATHS $ENV{LIBOGC} libogc ogc ${DEVKITPRO}/libogc ${DEVKITPRO}/ogc)

find_path(LIBOGC_INCLUDE_DIR gccore.h
          PATHS ${LIBOGC_PATHS}
          PATH_SUFFIXES include libogc/include ogc/include )

find_library(LIBCTRU_LIBRARY NAMES ctru libogc.a
          PATHS ${LIBOGC_PATHS}
          PATH_SUFFIXES lib libctru/lib )

set(LIBOGC_LIBRARIES ${LIBOGC_LIBRARY} )
set(LIBOGC_INCLUDE_DIRS ${LIBOGC_INCLUDE_DIR} )

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set LIBCTRU_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(LIBOG  DEFAULT_MSG
    LIBOGC_LIBRARY LIBOGC_INCLUDE_DIR)

mark_as_advanced(LIBOGC_INCLUDE_DIR LIBOGC_LIBRARY )
if(LIBOGC_FOUND)
    set(LIBOGC ${LIBOGC_INCLUDE_DIR}/..)
    message(STATUS "setting LIBOGC to ${LIBOGC}")

    add_library(wii::libogc STATIC IMPORTED GLOBAL)
    set_target_properties(wii::libogc PROPERTIES
        IMPORTED_LOCATION "${LIBOGC_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${LIBOGC_INCLUDE_DIR}"
    )
endif()
