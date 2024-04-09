cmake_minimum_required(VERSION 3.16)



############### Subdirectories for dependencies ###############



############### Names and Constants ###############
set(PROJECT_NAME Tobor)


############### Preprocessor Macros ###############



############### Project Declaration ###############
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)


#set(CMAKE_SUPPRESS_REGENERATION true) # CMAKE then does not create the target ZERO_CHECK, hence Visual Studio is able to generate class diagrams.

############### Programming Language and Compiler Flags ###############


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS False)

set(THREADS_PREFER_PTHREAD_FLAG ON)

if (WIN32 AND NOT (MSYS OR MINGW))
        set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /DNDEBUG") #what do all these flags mean?
        set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
        message("WIN32 Flags recognized")
endif()

if(MSVC)
  add_compile_options(/W4 /WX)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()



############### Libraries via include_directories ###############


############### Libraries via find_package ###############


############### Sources ###############
add_subdirectory(src)



############### Testing ###############


############### Visual Studio ###############
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})#Set Visualo Studio start-up project, so that one can directly run the debugger.
