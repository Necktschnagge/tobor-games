cmake_minimum_required(VERSION 3.16)



############### Subdirectories for dependencies ###############
set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)
add_subdirectory(extern/googletest)



############### Names and Constants ###############
set(PROJECT_NAME Tobor)
set(PROJECT_NAME_LIB ${PROJECT_NAME}_static_lib)
set(TEST_PROJECT_NAME TEST-${PROJECT_NAME})


if (DEFINED USE_QT6)
	message("Tobor:  Parameters:  USE_QT6 is defined:  ${USE_QT6}")
	if(USE_QT6)
		set(USE_QT6 TRUE)
	else()
		set(USE_QT6 FALSE)
	endif()
else()
	message("Tobor:  Parameters:  USE_QT6 not set")
	message("Tobor:  Parameters:  assuming USE_QT6 = TRUE")
	set(USE_QT6 TRUE)
endif()
message("Tobor:  Parameters:  USE_QT6 set to:  ${USE_QT6}")

ADD_DEFINITIONS(-DQT_NO_KEYWORDS) #turn off Qt's keywords: especially emit() does not work in combination with TBB

############### Preprocessor Macros ###############
add_compile_definitions(GIT_REPOSITORY_URL="https://github.com/Necktschnagge/tobor-games")



############### Project Declaration ###############
project(${PROJECT_NAME} VERSION 0.1 LANGUAGES CXX)


#set(CMAKE_SUPPRESS_REGENERATION true) # CMAKE then does not create the target ZERO_CHECK, hence Visual Studio is able to generate class diagrams.

############### Programming Language and Compiler Flags ###############
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS False)

set(THREADS_PREFER_PTHREAD_FLAG ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		message("Clang detected")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
endif()

if (MSVC)
        message("MSVC detected")
        set(CMAKE_CXX_FLAGS_RELEASE "/MT /O2 /Ob2 /DNDEBUG") #what do all these flags mean?
        set(CMAKE_CXX_FLAGS_DEBUG "/MTd /Zi /Ob0 /Od /RTC1")
endif()

if(MSVC)
  add_compile_options(/W4 /WX)
  add_compile_options(/bigobj)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()



############### Libraries via include_directories ###############
include_directories(SYSTEM extern/spdlog/include)
include_directories(SYSTEM extern/json/include)



############### Libraries via find_package ###############
find_package(Threads REQUIRED)

if(USE_QT6)
	find_package(QT NAMES Qt6 REQUIRED COMPONENTS Widgets Svg SvgWidgets)
	find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg SvgWidgets)
else()
	find_package(QT NAMES Qt5 REQUIRED COMPONENTS Widgets Svg)
	find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets Svg)
endif()

if(NOT MSVC)
	find_package(TBB REQUIRED)
endif()

############### Sources ###############
add_subdirectory(src)



############### Testing ###############
add_subdirectory(test)
enable_testing()



############### Visual Studio ###############
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})#Set Visualo Studio start-up project, so that one can directly run the debugger.
