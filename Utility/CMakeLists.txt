add_library(objUtility OBJECT
Types.hh STRINGIFY.hh
Checker.hh Exception.hh Internet_Checksum.hh
Checker.cc Exception.cc Internet_Checksum.cc)

set_target_properties(objUtility PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_library(Utility SHARED $<TARGET_OBJECTS:objUtility>)
add_library(sllUtility STATIC $<TARGET_OBJECTS:objUtility>)

if (WIN32)
   set_target_properties(sllUtility PROPERTIES OUTPUT_NAME libUtility)
	set_target_properties(Utility PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS ON)
	if(${CMAKE_VERSION} VERSION_GREATER_EQUAL "3.13")
		target_link_options(Utility PRIVATE "/MANIFEST:NO")
	else()
		set(CMAKE_SHARED_LINKER_FLAGS "/MANIFEST:NO")
	endif()
else()
	set_target_properties(sllUtility PROPERTIES OUTPUT_NAME Utility)
endif()

target_include_directories(Utility PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_SOURCE_DIR})
