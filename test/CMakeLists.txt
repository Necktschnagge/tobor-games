file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

# Link the static library FIRST so all dependencies propagate
target_link_libraries(${TEST_PROJECT_NAME} PRIVATE
    ${PROJECT_NAME_LIB}
    gtest
    gtest_main
)

add_test(NAME ${TEST_PROJECT_NAME} COMMAND ${TEST_PROJECT_NAME})
