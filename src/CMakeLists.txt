############### Find Source Files ###############
file(GLOB_RECURSE ALL_SOURCE_FILES LIST_DIRECTORIES true *.h *.cpp *.ui)



############### Target via add_library ###############
qt_add_library(${PROJECT_NAME_LIB} STATIC MANUAL_FINALIZATION ${ALL_SOURCE_FILES})

############### Libraries via target_link_libraries ###############
target_link_libraries(${PROJECT_NAME_LIB} PUBLIC
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Svg
	Qt${QT_VERSION_MAJOR}::SvgWidgets
)
target_link_libraries(${PROJECT_NAME_LIB} PUBLIC  Threads::Threads)
target_link_libraries(${PROJECT_NAME_LIB} PUBLIC  fmt::fmt-header-only)
target_link_libraries(${PROJECT_NAME_LIB} PUBLIC  spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME_LIB} PUBLIC  CLI11::CLI11)

if(NOT MSVC)
	target_link_libraries(${PROJECT_NAME_LIB} PUBLIC TBB::tbb)
endif()



############### Make sure depending targets inherit include directories ###############
target_include_directories(${PROJECT_NAME_LIB}
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)



############### Target Finalize ###############
qt_finalize_target(${PROJECT_NAME_LIB})



############### Target via add_executable ###############
if (WIN32)
	qt_add_executable(
		${PROJECT_NAME}
		MANUAL_FINALIZATION
		WIN32
	)
else()
	qt_add_executable(
		${PROJECT_NAME}
		MANUAL_FINALIZATION
	)
endif()



############### Link the executable to the library ###############
target_link_libraries(${PROJECT_NAME} PRIVATE
    ${PROJECT_NAME_LIB}
)



############### Target Properties ###############
set_target_properties(${PROJECT_NAME} PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)

set_property(TARGET ${PROJECT_NAME_LIB} PROPERTY MSVC_MP_FLAG "")
set_target_properties(${PROJECT_NAME_LIB} PROPERTIES UNITY_BUILD OFF)
set_target_properties(${PROJECT_NAME_LIB} PROPERTIES
    DISABLE_PRECOMPILE_HEADERS ON
)



############### Install executable ###############
install(TARGETS ${PROJECT_NAME}
	BUNDLE DESTINATION .
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)



############### Target Finalize ###############
qt_finalize_executable(${PROJECT_NAME})
