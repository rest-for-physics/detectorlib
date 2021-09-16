set( LibraryVersion "1.4" )
add_definitions(-DLIBRARY_VERSION="${LibraryVersion}")

#find garfield libs and includes
IF( NOT DEFINED REST_GARFIELD )
   set(REST_GARFIELD OFF)
ENDIF()

if (${REST_GARFIELD} MATCHES "ON")
    add_definitions(-DUSE_Garfield)
    include(FindGarfield)
    message(STATUS "GARFIELD " ${Garfield_INCLUDE_DIRS})
    set(external_include_dirs ${external_include_dirs} ${Garfield_INCLUDE_DIRS})
    set(external_libs "${external_libs} ${Garfield_LIBRARIES}")
    set(external_include_dirs ${external_include_dirs} ${Garfield_DIR}/Heed)
    message(STATUS "HEED Include " ${Garfield_DIR}/Heed)
    message(STATUS "ALL INCLUDES: " ${external_include_dirs})
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
