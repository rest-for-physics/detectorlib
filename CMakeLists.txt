set(LibraryVersion "1.5")
add_definitions(-DLIBRARY_VERSION="${LibraryVersion}")

#find garfield libs and includes
IF (NOT DEFINED REST_GARFIELD)
    set(REST_GARFIELD OFF)
ENDIF ()

if (${REST_GARFIELD} MATCHES "ON")
    add_definitions(-DUSE_Garfield)
    include(FindGarfield)
    if (NOT ${Garfield_INCLUDE_DIRS})
        if (DEFINED ENV{GARFIELD_INSTALL})
            set(GARFIELD_INSTALL "$ENV{GARFIELD_INSTALL}")
            set(Garfield_INCLUDE_DIRS "${GARFIELD_INSTALL}/include/Garfield")
            set(Garfield_DIR "${GARFIELD_INSTALL}/share")
            find_library(Garfield_LIBRARIES NAMES libGarfield.so Garfield
                    HINTS ${GARFIELD_INSTALL}/lib)
        endif ()
    endif ()
    message(STATUS "GARFIELD " ${Garfield_INCLUDE_DIRS})
    set(external_include_dirs ${external_include_dirs} ${Garfield_INCLUDE_DIRS})
    set(external_libs "${external_libs} ${Garfield_LIBRARIES}")
    set(feature_added "Garfield")
    set(feature_added ${feature_added} PARENT_SCOPE)
else ()
    set(REST_GARFIELD OFF)
endif (${REST_GARFIELD} MATCHES "ON")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes ${excludes} TRestDetectorHitsEventViewer)
endif ()

COMPILELIB("")

file(GLOB_RECURSE MAC "${CMAKE_CURRENT_SOURCE_DIR}/macros/*")
INSTALL(FILES ${MAC} DESTINATION ./macros/detector)
