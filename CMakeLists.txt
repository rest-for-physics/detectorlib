set(LibraryVersion "1.6")
add_definitions(-DLIBRARY_VERSION="${LibraryVersion}")

#find garfield libs and includes
IF (NOT DEFINED REST_GARFIELD)
    set(REST_GARFIELD OFF)
ENDIF ()

if (${REST_GARFIELD} MATCHES "ON")
    add_definitions(-DUSE_Garfield)

    if (DEFINED ENV{GARFIELD_INSTALL})
        # Tested for Garfield af4a14516489babbd6e6df780edd58fedf7fa12b
        message(STATUS "Using new Garfield CMake")
        set(GARFIELD_INSTALL $ENV{GARFIELD_INSTALL})
        find_package(Garfield REQUIRED)
        set(external_include_dirs ${external_include_dirs} ${GARFIELD_INSTALL}/include/Garfield)
        set(external_libs ${external_libs} Garfield::Garfield)
        # recommended CMake way does not work, we need to delete the "FindGarfield.cmake" script
    elseif (DEFINED ENV{GARFIELD_HOME})
        # Old way, for backwards compatibility
        message(STATUS "Using old Garfield CMake")
        set(GARFIELD_INSTALL $ENV{GARFIELD_HOME})
        include(FindGarfieldOld)
        message(STATUS Garfield_INCLUDE_DIRS ${Garfield_INCLUDE_DIRS})
        message(STATUS Garfield_INCLUDE_DIRS ${Garfield_INCLUDE_Heed_DIRS})
        message(STATUS Garfield_LIBRARIES ${Garfield_LIBRARIES})
        set(external_include_dirs ${external_include_dirs} ${Garfield_INCLUDE_DIRS})
        set(external_include_dirs ${external_include_dirs} ${Garfield_INCLUDE_Heed_DIRS})
        set(external_libs ${external_libs} ${Garfield_LIBRARIES})
        add_definitions(-DUSE_Garfield_OLD)
    else ()
        message(FATAL_ERROR "Garfield not found")
    endif ()

    #set(GARFIELD_INSTALL $ENV{GARFIELD_INSTALL})
    message(STATUS "GARFIELD INSTALLATION: ${GARFIELD_INSTALL}")

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

ADD_TEST()
