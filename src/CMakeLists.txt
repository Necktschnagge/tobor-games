############### Find Source Files ###############
file(GLOB_RECURSE ALL_SOURCE_FILES LIST_DIRECTORIES true *.h *.cpp *.ui)



############### Target via add_executable ###############



############### Libraries via target_link_libraries ###############

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)



############### Target Properties ###############


############### Target Finalize ###############


############### Target via add_library ###############


############### Libraries via target_link_libraries ###############



############### Target Properties ###############


############### Target Finalize ###############
