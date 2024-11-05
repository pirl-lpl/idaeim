add_library(objStrings OBJECT
Line_Count_Filter.cc Sliding_String.cc String.cc VMS_Records_Filter.cc)

set(headers String_Filter.hh Line_Count_Filter.hh Sliding_String.hh String.hh VMS_Records_Filter.hh)

set_target_properties(objStrings PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Strings SHARED $<TARGET_OBJECTS:objStrings>)
add_library(sllStrings STATIC $<TARGET_OBJECTS:objStrings>)

if (WIN32)
	set_target_properties(Strings PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
	set_target_properties(sllStrings PROPERTIES OUTPUT_NAME libStrings)

	if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13")
		target_link_options(Strings PRIVATE "/MANIFEST:NO")
	else()
		set(CMAKE_SHARED_LINKER_FLAGS "/MANIFEST:NO")
	endif()
	#
	# TODO this hack copies the static lib over the import lib
	# because the latter doesn't have symbols for static data fields
	add_custom_command(TARGET Strings POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sllStrings> Strings.lib)

else()
	set_target_properties(sllStrings PROPERTIES OUTPUT_NAME Strings)
endif()

target_include_directories(Strings PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})

set_target_properties(Strings PROPERTIES PUBLIC_HEADER "${headers}")

include(GNUInstallDirs)
install(TARGETS Strings sllStrings
    EXPORT ${PROJECT_NAME}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Strings
)

install(FILES Strings.hh DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})