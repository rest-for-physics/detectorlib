root_generate_dictionary(G__RestLegacyRecoveryFixture LegacyFixture.h LINKDEF
                         LinkDef.h)
add_executable(
    RestLegacyRecoveryFixtureWriter
    WriteFixture.cxx LegacyFixture.cxx
    ${CMAKE_CURRENT_BINARY_DIR}/G__RestLegacyRecoveryFixture.cxx)
target_include_directories(RestLegacyRecoveryFixtureWriter
                           PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(RestLegacyRecoveryFixtureWriter PRIVATE RestFramework
                                                              ${ROOT_LIBRARIES})

add_executable(RestLegacyRecoveryFixtureReader ReadFixture.cxx)
target_link_libraries(RestLegacyRecoveryFixtureReader
                      PRIVATE RestDetector RestFramework ${ROOT_LIBRARIES})

add_dependencies(testRestDetector RestLegacyRecoveryFixtureWriter
                 RestLegacyRecoveryFixtureReader restRoot)
target_compile_definitions(
    testRestDetector
    PRIVATE
        REST_LEGACY_FIXTURE_WRITER="$<TARGET_FILE:RestLegacyRecoveryFixtureWriter>"
        REST_LEGACY_FIXTURE_READER="$<TARGET_FILE:RestLegacyRecoveryFixtureReader>"
        REST_LEGACY_RESTROOT="$<TARGET_FILE:restRoot>"
        REST_LEGACY_MACRO="${CMAKE_SOURCE_DIR}/macros/legacy/recoverLegacySignalData.C"
        REST_LEGACY_WRONG_INTERMEDIATE="${CMAKE_CURRENT_SOURCE_DIR}/WrongIntermediate.C"
)
