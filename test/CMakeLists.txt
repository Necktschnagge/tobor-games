file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

# Link the static library FIRST so all dependencies propagate
target_link_libraries(${TEST_PROJECT_NAME} PRIVATE
    ${PROJECT_NAME_LIB}
    gtest
    gtest_main
	nlohmann_json::nlohmann_json
)


# Disable MSVC multi-processor compilation for test target
set_property(TARGET ${TEST_PROJECT_NAME} PROPERTY MSVC_MP_FLAG "")

# Disable unity builds for test target
set_target_properties(${TEST_PROJECT_NAME} PROPERTIES UNITY_BUILD OFF)

# Disable precompiled headers for test target
set_target_properties(${TEST_PROJECT_NAME} PROPERTIES DISABLE_PRECOMPILE_HEADERS ON)


add_test(NAME ${TEST_PROJECT_NAME} COMMAND ${TEST_PROJECT_NAME})
