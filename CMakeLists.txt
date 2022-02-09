set(LibraryVersion "1.5")
add_definitions(-DLIBRARY_VERSION="${LibraryVersion}")

# find garfield libs and includes
IF (NOT DEFINED REST_GARFIELD)
    set(REST_GARFIELD OFF)
ENDIF ()

if (${REST_GARFIELD} MATCHES "ON")
    add_definitions(-DUSE_Garfield)
    find_package(Garfield REQUIRED)
    set(GARFIELD_INSTALL $ENV{GARFIELD_INSTALL})
    message(STATUS "GARFIELD INSTALLATION: ${GARFIELD_INSTALL}")
    set(external_include_dirs ${external_include_dirs} ${GARFIELD_INSTALL}/include/Garfield)
    set(external_libs ${external_libs} "-L${GARFIELD_INSTALL}/lib -lGarfield")

    message(STATUS "ALL INCLUDES: " ${external_include_dirs})

    set(feature_added "Garfield")
    set(feature_added ${feature_added} PARENT_SCOPE)

    install(DIRECTORY ${GARFIELD_INSTALL}/include/ DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
else ()
    set(REST_GARFIELD OFF)
endif (${REST_GARFIELD} MATCHES "ON")

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes ${excludes} TRestDetectorHitsEventViewer)
endif ()

COMPILELIB("")

file(GLOB_RECURSE MAC "${CMAKE_CURRENT_SOURCE_DIR}/macros/*")
INSTALL(FILES ${MAC} DESTINATION ./macros/detector)
