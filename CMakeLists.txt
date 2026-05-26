cmake_minimum_required(VERSION 3.16)

set(CMAKE_WIN32_EXECUTABLE OFF)


############### Subdirectories for dependencies ###############
set(gtest_force_shared_crt ON CACHE BOOL "Always use msvcrt.dll" FORCE)


# Since we use fmt directly and spdlog -> fmt, we need to tell spdlog to use our fmt version.

#set(SPDLOG_FMT_EXTERNAL ON CACHE BOOL "" FORCE) # use external binary
set(SPDLOG_FMT_EXTERNAL_HO ON CACHE BOOL "" FORCE) # use external header only

set(FMT_TEST OFF CACHE BOOL "" FORCE)
set(FMT_DOC OFF CACHE BOOL "" FORCE)

set(JSON_BuildTests OFF CACHE INTERNAL "")

add_subdirectory(extern/fmt)
add_subdirectory(extern/spdlog)

add_subdirectory(extern/json)

add_subdirectory(extern/CLI11)

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
project(${PROJECT_NAME} VERSION 1.1 LANGUAGES CXX)


#set(CMAKE_SUPPRESS_REGENERATION true) # CMAKE then does not create the target ZERO_CHECK, hence Visual Studio is able to generate class diagrams.



############### Programming Language and Compiler Flags ###############
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS False)

set(THREADS_PREFER_PTHREAD_FLAG ON)

if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang"  AND NOT APPLE)
		message("Clang detected")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")
endif()

if (MSVC)
        message("MSVC detected")
		# Optional: keep your optimization flags, but DO NOT override runtime library
		set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /Ob2")
		set(CMAKE_CXX_FLAGS_DEBUG   "${CMAKE_CXX_FLAGS_DEBUG} /Zi /Ob0 /Od /RTC1")
endif()

if(MSVC)
  add_compile_options(/W4 /WX /MP)
  add_compile_options(/bigobj)
else()
  add_compile_options(-Wall -Wextra -pedantic -Werror)
endif()



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

#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

message("CMAKE BINARY DIR:   ${CMAKE_BINARY_DIR}")
message("CMAKE_RUNTIME_OUTPUT_DIRECTORY :   ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
message("CMAKE_SOURCE_DIR :   ${CMAKE_SOURCE_DIR}")

