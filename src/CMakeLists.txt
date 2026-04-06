############### Find Source Files ###############
file(GLOB_RECURSE ALL_SOURCE_FILES LIST_DIRECTORIES true *.h *.cpp *.ui)



############### Target via add_executable ###############
if (WIN32)
	qt_add_executable(
		${PROJECT_NAME}
		MANUAL_FINALIZATION
		WIN32
		${ALL_SOURCE_FILES}
	)
else()
	qt_add_executable(
		${PROJECT_NAME}
		MANUAL_FINALIZATION
		${ALL_SOURCE_FILES}
	)
endif()



############### Libraries via target_link_libraries ###############
target_link_libraries(${PROJECT_NAME} PRIVATE
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Svg
	Qt${QT_VERSION_MAJOR}::SvgWidgets
)

target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME} PRIVATE fmt::fmt-header-only)
target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME} PRIVATE CLI11::CLI11)

if(NOT MSVC)
	target_link_libraries(${PROJECT_NAME} PRIVATE TBB::tbb)
endif()




############### Target Properties ###############
set_target_properties(${PROJECT_NAME} PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)



install(TARGETS ${PROJECT_NAME}
	BUNDLE DESTINATION .
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)



############### Target Finalize ###############
if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(${PROJECT_NAME})
endif()



############### Target via add_library ###############
if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
	qt_add_library(${PROJECT_NAME_LIB} STATIC MANUAL_FINALIZATION ${ALL_SOURCE_FILES})
else()
	add_library(${PROJECT_NAME_LIB} STATIC ${ALL_SOURCE_FILES}) #for using in tests
endif()



############### Libraries via target_link_libraries ###############
if(USE_QT6)
    target_link_libraries(${PROJECT_NAME_LIB} PRIVATE
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Svg
	Qt${QT_VERSION_MAJOR}::SvgWidgets
	)
else()
    target_link_libraries(${PROJECT_NAME_LIB} PRIVATE
	Qt${QT_VERSION_MAJOR}::Widgets
	Qt${QT_VERSION_MAJOR}::Svg
	)
endif()

target_link_libraries(${PROJECT_NAME_LIB} PRIVATE Threads::Threads)
target_link_libraries(${PROJECT_NAME_LIB} PRIVATE fmt::fmt-header-only)
target_link_libraries(${PROJECT_NAME_LIB} PRIVATE spdlog::spdlog_header_only)
target_link_libraries(${PROJECT_NAME_LIB} PRIVATE CLI11::CLI11)

if(NOT MSVC)
	target_link_libraries(${PROJECT_NAME_LIB} PRIVATE TBB::tbb)
endif()


############### Target Properties ###############
set_target_properties(${PROJECT_NAME_LIB} PROPERTIES
	MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
	MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
	MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
	MACOSX_BUNDLE TRUE
	WIN32_EXECUTABLE TRUE
)



install(TARGETS ${PROJECT_NAME_LIB}
	BUNDLE DESTINATION .
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)



############### Target Finalize ###############
if(QT_VERSION_MAJOR EQUAL 6)
	qt_finalize_executable(${PROJECT_NAME_LIB})
endif()
