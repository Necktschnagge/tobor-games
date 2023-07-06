file(GLOB_RECURSE TEST_SOURCES LIST_DIRECTORIES false *.h *.cpp)

#set(SOURCES ${TEST_SOURCES})

add_executable(${TEST_PROJECT_NAME} ${TEST_SOURCES})

add_test(NAME ${TEST_PROJECT_NAME} COMMAND ${TEST_PROJECT_NAME}) # what does command mean in detail?

############### Libraries ###############
if(LINUX)
	target_link_libraries(${TEST_PROJECT_NAME} PUBLIC
		pthread
	)
endif()

target_link_libraries(${TEST_PROJECT_NAME} PUBLIC ${PROJECT_NAME_LIB} gtest)
