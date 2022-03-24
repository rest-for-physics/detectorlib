set(LibraryVersion "1.6")
add_definitions(-DLIBRARY_VERSION="${LibraryVersion}")

if (NOT DEFINED REST_GARFIELD OR NOT REST_GARFIELD)
    set(REST_GARFIELD OFF)
    message(STATUS "Garfield DISABLED")
endif ()

if (REST_GARFIELD)
    message(STATUS "Garfield ENABLED")

    add_definitions(-DUSE_Garfield)

    # Tested for Garfield af4a14516489babbd6e6df780edd58fedf7fa12b
    find_package(Garfield REQUIRED)

    set(GARFIELD_INSTALL $ENV{GARFIELD_INSTALL})
    set(external_include_dirs ${external_include_dirs} ${GARFIELD_INSTALL}/include/Garfield)
    set(external_libs ${external_libs} Garfield::Garfield)

    message(STATUS "GARFIELD INSTALLATION: ${GARFIELD_INSTALL}")

    set(feature_added "Garfield")
    set(feature_added ${feature_added} PARENT_SCOPE)

endif ()

if (NOT ${REST_EVE} MATCHES "ON")
    set(excludes ${excludes} TRestDetectorHitsEventViewer)
endif ()

COMPILELIB("")

file(GLOB_RECURSE MAC "${CMAKE_CURRENT_SOURCE_DIR}/macros/*")
INSTALL(FILES ${MAC} DESTINATION ./macros/detector)
